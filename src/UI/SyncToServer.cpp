#include "UI/SyncToServer.h"
#include <filesystem>
#include "libssh/libssh.h"
#include "libssh/sftp.h"
#include "ssh_helper.h"
#include "utils.h"
#include <fcntl.h>
#include <iostream>

namespace bakermaker {
    SyncToServer::SyncToServer()
            : BaseUIScreen(bakermaker::ProgramStage::SYNC_TO_SERVER, &bakermaker::configScreens),
              exec(nullptr), execDone(false), success(0) {

    }

    void SyncToServer::render() {
        ImGui::PushFont(fontlist[0]);
        ImGui::Text("Sync to server");
        ImGui::PopFont();

        if(exec && !execDone) ImGui::BeginDisabled();

        if(ImGui::Button("Sync files from server")) {
            execDone = false;
            success = 0;
            config["synced"] = false;
            exec = new std::thread(&SyncToServer::syncFrom, this);
            ImGui::BeginDisabled();
        }

        ImGui::SameLine();

        if(ImGui::Button("Save changes to server")) {}

        if(exec && !execDone) {
            ImGui::EndDisabled();
            bakermaker::spinner();
        }

        if(execDone) {
            if(exec) {
                exec->join();
                delete exec;
                exec = nullptr;

                if(success != 0)
                    bakermaker::startErrorModal(
                            (ST::string("aaaaaaah") + std::to_string(success)).c_str());
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
            success = -2;
            execDone = true;
            return;
        }

        while((atr = sftp_readdir(sftp, dir)) != nullptr) {
            if(strcmp(atr->name, ".") == 0 || strcmp(atr->name, "..") == 0) continue;
            sftp_file file = sftp_open(sftp, (ST::string("gitolite-admin/keydir/") + atr->name).c_str(),
                                       O_RDONLY, 0);

            if(file == nullptr) {
                sftp_closedir(dir);
                sftp_free(sftp);
                ssh_disconnect(sess);
                ssh_free(sess);

                success = -3;
                execDone = true;
            }

            FILE* key;

            fopen_s(&key, (ST::string("keys/") + atr->name).c_str(), "w");

            while(true) {
                char buf[SFTP_BUF_SIZE] = {'\0'};
                ssize_t nbytes = sftp_read(file, buf, SFTP_BUF_SIZE);
                if(nbytes == 0) break;
                else if(nbytes < 0) {
                    sftp_attributes_free(atr);
                    fclose(key);
                    sftp_close(file);
                    sftp_closedir(dir);
                    sftp_free(sftp);
                    ssh_disconnect(sess);
                    ssh_free(sess);


                    success = ssh_get_error_code(sess);
                    execDone = true;
                    return;
                }

                size_t nwritten = fwrite(buf, 1, nbytes, key);

                if(nwritten != nbytes) {
                    sftp_attributes_free(atr);
                    fclose(key);
                    sftp_close(file);
                    sftp_closedir(dir);
                    sftp_free(sftp);
                    ssh_disconnect(sess);
                    ssh_free(sess);

                    success = -5;
                    execDone = true;
                    return;
                }
            }

            sftp_attributes_free(atr);
            fclose(key);
            sftp_close(file);
        }

        sftp_closedir(dir);

        for(const auto& file : std::filesystem::directory_iterator("keys")) {
            if(!file.path().string().ends_with(".pub")) continue;
        }

        sftp_free(sftp);
        ssh_disconnect(sess);
        ssh_free(sess);

        success = 0;
        execDone = true;
    }

    void SyncToServer::syncTo() {}
}
