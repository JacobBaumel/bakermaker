#include "UI/SyncToServer.h"
#include "UI/RepoManage.h"
#include <filesystem>
#include "libssh/libssh.h"
#include "libssh/sftp.h"
#include "ssh_helper.h"
#include "utils.h"
#include <iostream>
#include <set>

namespace bakermaker {
    SyncToServer::SyncToServer()
            : BaseUIScreen(bakermaker::ProgramStage::SYNC_TO_SERVER, &bakermaker::configScreens),
              exec(nullptr), execDone(false), success(0), status(""), statusmutex() {

    }

    void SyncToServer::render() {
        ImGui::PushFont(fontlist[0]);
        ImGui::Text("Sync to server");
        ImGui::PopFont();

        if(exec && !execDone) ImGui::BeginDisabled();

        if(ImGui::Button("Sync files from server")) {
            execDone = false;
            success = 0;
            status = "Syncing from server";
            config["synced"] = false;
            exec = new std::thread(&SyncToServer::syncFrom, this);
            ImGui::BeginDisabled();
        }

        ImGui::SameLine();

        if(ImGui::Button("Save changes to server")) {}

        if(exec && !execDone) {
            ImGui::EndDisabled();
            statusmutex.lock();
            ImGui::Text("%s", status.c_str());
            statusmutex.unlock();
            ImGui::SameLine();
            bakermaker::spinner();
        }

        if(execDone) {
            if(exec) {
                exec->join();
                delete exec;
                exec = nullptr;

                if(success < 0) {
                    bakermaker::startErrorModal(
                            (ST::string("Sync failed with error ") + std::to_string(success) +
                             '\n' + status).c_str());
                }

                if(success == 1) {
                    bakermaker::startErrorModal((ST::string("Sync was successful, with status:\n\n") + status).c_str());
                }
            }
        }

    }

    void SyncToServer::syncFrom() {
        for(const auto& file: std::filesystem::directory_iterator("keys")) {
            if(file.path().string().ends_with(".pub")) std::filesystem::remove(file);
        }

        ssh_session sess;
        memset((void*) &sess, 0, sizeof(ssh_session));
        {
            int rc = bakermaker::createSession(sess,
                                               config["server"]["ip"].get<std::string>().c_str(),
                                               config["server"]["user"].get<std::string>().c_str(),
                                               config["server"]["keyfile"].get<std::string>().c_str(),
                                               config["server"]["port"].get<int>());

            if(rc != SSH_OK) {
                statusmutex.lock();
                status = "Failed to create ssh session";
                statusmutex.unlock();
                success = -1;
                execDone = true;
                return;
            }
        }

        sftp_session sftp = sftp_new(sess);
        sftp_init(sftp);

        sftp_dir dir;
        sftp_attributes atr;

        dir = sftp_opendir(sftp, "gitolite-admin/keydir");

        if(!dir) {
            statusmutex.lock();
            status = "Failed to open remote directory";
            statusmutex.unlock();
            sftp_free(sftp);
            ssh_disconnect(sess);
            ssh_free(sess);
            success = -2;
            execDone = true;
            return;
        }

        while((atr = sftp_readdir(sftp, dir)) != nullptr) {
            if(strcmp(atr->name, ".") == 0 || strcmp(atr->name, "..") == 0) continue;
            int rc = downloadToLocal(sftp,
                                     (ST::string("gitolite-admin/keydir/") + atr->name).c_str(),
                                     (ST::string("keys/") + atr->name).c_str());

            if(rc != 0) {
                setStatus(rc);
                sftp_attributes_free(atr);
                sftp_closedir(dir);
                sftp_free(sftp);
                ssh_disconnect(sess);
                ssh_free(sess);
                success = -3;
                execDone = true;
                return;
            }

            sftp_attributes_free(atr);
        }

        sftp_closedir(dir);

        std::set<ST::string> files;
        for(const auto& file: std::filesystem::directory_iterator("keys"))
            files.insert(file.path().string().c_str());

        std::set<ST::string> nopriv;
        std::set<ST::string> nopub;

        for(const auto& str: files) {
            if(str.ends_with(".pub")) {
                ST::string subed = str.substr(0, str.size() - 4);
                if(files.count(subed) == 0)
                    nopriv.insert(subed);
            }

            else if(files.count(str + ".pub") == 0) {
                nopub.insert(str);

                statusmutex.lock();
                status = "Generating public key for ";
                status += str;
                statusmutex.unlock();

                ssh_key prkey;
                if(SSH_OK !=
                   ssh_pki_import_privkey_file(str.c_str(), "", nullptr,
                                               nullptr, &prkey)) {
                    statusmutex.lock();
                    status = "Failed to read private key: " + str;
                    statusmutex.unlock();
                    sftp_free(sftp);
                    ssh_disconnect(sess);
                    ssh_free(sess);
                    success = -6;
                    execDone = true;
                    return;
                }

                ssh_key pubkey;
                ssh_pki_export_privkey_to_pubkey(prkey, &pubkey);
                if(SSH_OK != ssh_pki_export_pubkey_file(pubkey, (str + ".pub").c_str())) {
                    statusmutex.lock();
                    status = "Failed to write public key: " + str + ".pub";
                    statusmutex.unlock();
                    sftp_free(sftp);
                    ssh_disconnect(sess);
                    ssh_free(sess);
                    success = -7;
                    execDone = true;
                    return;
                }
            }
        }

        for(const auto& str : nopriv) files.erase(str);
        std::erase_if(files, [](const ST::string& str) { return str.ends_with(".pub"); });
        config["keys"] = files;

        {
            int rc = downloadToLocal(sftp, "gitolite-admin/conf/gitolite.conf", "gitolite.conf");

            if(rc != 0) {
                setStatus(rc);
                sftp_free(sftp);
                ssh_disconnect(sess);
                ssh_free(sess);
                success = -8;
                execDone = true;
                return;
            }
        }

        ((bakermaker::RepoManage*) bakermaker::configScreens[bakermaker::ProgramStage::REPO_MANAGE])->reset();

        sftp_free(sftp);
        ssh_disconnect(sess);
        ssh_free(sess);

        success = 0;
        status.clear();

        if(!nopriv.empty()) {
            success = 1;
            status += "The following public keys were present on the server, but their corresponding private keys were not"
                      "present on this computer. These keys may be usable, if the private key still exists:\n";
            for(const auto& key: nopriv) {
                std::cout << key.c_str() << std::endl;
                status += key + '\n';
            }

            status += "\n\n";
        }

        if(!nopub.empty()) {
            success = 1;
            status += "The following users were not registered with the server, but did have private keys present on"
                      " this computer. Public keys were created and will be uploaded on the next save:\n";
            for(const auto& key: nopub) {
                status += key + '\n';
            }
        }

        execDone = true;
    }

    void SyncToServer::syncTo() {}

    void SyncToServer::setStatus(int rc) {
        statusmutex.lock();
        switch(rc) {
            case -1:
                status = "Failed to open remote file";
                break;

            case -2:
                status = "Failed to open local file";
                break;

            case -3:
                status = "Failed to read remote file";
                break;

            case -4:
                status = "Failed to write to local file";
                break;
        }

        statusmutex.unlock();
    }

}