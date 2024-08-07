#include <filesystem>
#include <set>

#include "libssh/libssh.h"
#include "libssh/sftp.h"

#include "UI/RepoManage.h"
#include "ssh_helper.h"
#include "UI/SyncToServer.h"
#include "utils.h"

namespace bakermaker {
    using namespace ST::literals;

    SyncToServer::SyncToServer() : BaseUIScreen(ProgramStage::SYNC_TO_SERVER, &configScreens), exec(nullptr),
                                   execDone(false), success(0), status("") {
    }

    void SyncToServer::render() {
        // Header
        ImGui::PushFont(fontlist[1]);
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

        if(ImGui::Button("Save changes to server")) {
            startSync("");
            ImGui::BeginDisabled();
        }

        if(config["unsaved"].get<bool>()) {
            ImGui::NewLine();
            ImGui::PushFont(fontlist[2]);
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 0, 0, 1));
            ImGui::Text("WARNING: You have unsaved changes. Either save changes to server, or reset by syncing.");
            ImGui::PopStyleColor();
            ImGui::PopFont();
        }

        if(exec && !execDone) {
            ImGui::EndDisabled();
            statusmutex.lock();
            ImGui::Text("%s", status.c_str());
            statusmutex.unlock();
            ImGui::SameLine();
            spinner();
        }

        if(execDone) {
            if(exec) {
                exec->join();
                delete exec;
                exec = nullptr;

                if(success < 0) {
                    startErrorModal(
                        ("Sync failed with error: "_st + std::to_string(success) + "\n\n" + status).c_str());
                }

                else if(success > 0) {
                    startErrorModal(
                        ("Save failed with error: "_st + std::to_string(success) + "\n\n" + status).c_str());
                }

                else {
                    config["synced"] = true;
                    config["unsaved"] = false;
                }
            }

            else if(success == 0) {
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0, 1, 0, 1));
                ImGui::Text("Success!");
                ImGui::PopStyleColor();
            }
        }
    }

    void SyncToServer::syncFrom() {
        ssh_session sess = {};
        {
            int rc = createSession(sess,
                                   config["server"]["ip"].get<std::string>().c_str(),
                                   config["server"]["user"].get<std::string>().c_str(),
                                   "./keyfile",
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

        sftp_dir dir = sftp_opendir(sftp, "gitolite-admin/keydir");
        sftp_attributes atr;

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
            int rc = downloadToLocal(sftp, ("gitolite-admin/keydir/"_st + atr->name).c_str(),
                                     ("keys/"_st + atr->name).c_str());

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
        for(const auto& file : std::filesystem::directory_iterator("keys"))
            files.insert(file.path().string().substr(5).c_str());

        std::set<ST::string> nopriv;
        std::set<ST::string> nopub;

        for(const auto& str : files) {
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

        if(0 != ((RepoManage*)configScreens[ProgramStage::REPO_MANAGE])->reset()) {
            sftp_free(sftp);
            ssh_disconnect(sess);
            ssh_free(sess);
            success = -10;
            execDone = true;
            return;
        }

        sftp_free(sftp);
        ssh_disconnect(sess);
        ssh_free(sess);

        success = 0;
        status.clear();

        if(!nopriv.empty()) {
            status +=
                "The following public keys were present on the server, but their corresponding private keys were not"
                "present on this computer. These keys may be usable, if the private key still exists:\n";
            for(const auto& key : nopriv) {
                status += key + '\n';
            }

            status += "\n\n";
        }

        if(!nopub.empty()) {
            status += "The following users were not registered with the server, but did have private keys present on"
                " this computer. Public keys were created and will be uploaded on the next save:\n";
            for(const auto& key : nopub) {
                status += key + '\n';
            }
        }

        execDone = true;
    }

    void SyncToServer::syncTo(const ST::string deleterepo) {
        ssh_session sess = {};

        int rc = createSession(sess, config["server"]["ip"].get<std::string>().c_str(),
                               config["server"]["user"].get<std::string>().c_str(),
                               "./keyfile",
                               config["server"]["port"].get<int>());

        if(rc != SSH_OK) {
            statusmutex.lock();
            status = "Failed to create ssh session";
            statusmutex.unlock();
            success = -1;
            execDone = true;
            return;
        }

        sftp_session sftp = sftp_new(sess);
        sftp_init(sftp);

        runCommand(sess, "rm gitolite-admin/keydir/*.pub");

        for(const auto& file : std::filesystem::directory_iterator("keys")) {
            ST::string path = file.path().string().c_str();
            if(!path.ends_with(".pub")) continue;
            statusmutex.lock();
            status = "Saving changes to server: Uploading "_st + path;
            statusmutex.unlock();
            if(0 != uploadToRemote(sftp, path.c_str(), ("gitolite-admin/keydir/"_st + path.substr(5)).c_str())) {
                success = -2;
                sftp_free(sftp);
                ssh_disconnect(sess);
                ssh_free(sess);
                execDone = true;
                return;
            }
        }

        statusmutex.lock();
        status = "Saving changes to server: Uploading gitolite.conf";
        statusmutex.unlock();

        if(0 != uploadToRemote(sftp, "gitolite.conf", "gitolite-admin/conf/gitolite.conf")) {
            success = -3;
        }

        statusmutex.lock();
        status = "Committing changes";
        statusmutex.unlock();
        runCommand(sess, "~/commitall.sh");

        if(!deleterepo.empty()) {
            statusmutex.lock();
            status = "Deleting repository "_st + deleterepo;
            statusmutex.unlock();
            rc = runCommand(sess, ("sudo rm -rf /home/git/repositories/"_st + deleterepo + ".git"_st).c_str());
            if(rc != 0) success = -4;
        }

        sftp_free(sftp);
        ssh_disconnect(sess);
        ssh_free(sess);

        status.clear();
        execDone = true;
    }

    void SyncToServer::startSync(const ST::string& deleterepo) {
        if(exec) return;
        execDone = false;
        success = 0;
        status = "Saving to server";
        config["synced"] = false;
        ((RepoManage*) configScreens[ProgramStage::REPO_MANAGE])->save();
        exec = new std::thread(&SyncToServer::syncTo, this, deleterepo);
    }

    void SyncToServer::setStatus(const int rc) {
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

        default:
            status = "";
            break;
        }

        statusmutex.unlock();
    }
}
