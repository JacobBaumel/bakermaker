#include "UI/ServerInstall.h"
#include "romfs/romfs.hpp"
#include "utils.h"
#include "libssh/libssh.h"
#include "libssh/sftp.h"
#include "ssh_helper.h"

#include <iostream>

namespace bakermaker {
    ServerInstall::ServerInstall() : BaseUIScreen(bakermaker::ProgramStage::SERVER_INSTALL, &bakermaker::setupScreens),
                                     execDone(false), execProgress(0), bufferMutex(), hasStartedExec(false),
                                     showCommandOutputs(false),
                                     exec(nullptr) {
        romfs::Resource md = romfs::get("ServerInstallText.md");
        instructions = ST::string((const char*) md.data(), md.size());
    }

    void ServerInstall::render() {
        ImGui::PushFont(fontlist[1]);
        ImGui::Text("Server Software Install");
        ImGui::PopFont();
        ImGui::Separator();

        bakermaker::documentation->render(instructions);

        ImGui::NewLine();

        if(config["keys"].empty()) ImGui::BeginDisabled();

        if(ImGui::Button("Begin##server_install_begin")) {
            hasStartedExec = true;

            exec = new std::thread([this]() {
                ssh_session ubuntu;
                memset((void*) &ubuntu, 0, sizeof(ssh_session));
                {
                    int rc = bakermaker::createSession(ubuntu, config["server"]["ip"].get<std::string>().c_str(),
                                                       config["server"]["user"].get<std::string>().c_str(),
                                                       config["server"]["keyfile"].get<std::string>().c_str());

                    if(rc != SSH_OK) {
                        return;
                    }
                }

                sftp_session sftp;
                sftp = sftp_new(ubuntu);
                sftp_init(sftp);

                uploadToRemote(sftp, (ST::string("keys/") + config["keys"][0].get<std::string>()).c_str(),
                               "gito");

                uploadToRemote(sftp, (ST::string("keys/") + config["keys"][0].get<std::string>() + ".pub")
                        .c_str(), "authorized_keys");

                {
                    romfs::Resource script = romfs::get("install.sh");
                    uploadToRemote(sftp, (void*) script.data(), script.size(), "install.sh");
                    script = romfs::get("commitall.sh");
                    uploadToRemote(sftp, (void*) script.data(), script.size(), "commitall.sh");
                }

                sftp_free(sftp);

                runSSHCommand(ubuntu, "chmod +x ~/install.sh");
                runSSHCommand(ubuntu, "chmod +x ~/commitall.sh");
                runSSHCommand(ubuntu, "~/install.sh");

                ssh_disconnect(ubuntu);
                ssh_free(ubuntu);
            });
        }

        if(config["keys"].empty()) {
            ImGui::EndDisabled();
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 0, 0, 1));
            ImGui::Text("Create admin key first!");
            ImGui::PopStyleColor();
        }

        if(hasStartedExec) {
            if(ImGui::Button("Show/Hide Command Outputs##serverinstall_commandoutputs")) {
                showCommandOutputs = !showCommandOutputs;
            }

            if(showCommandOutputs) {
                ImGui::SetNextWindowSize(ImVec2(500, 500), ImGuiCond_FirstUseEver);
                if(ImGui::Begin("Command Output")) {
                    ImGui::PushFont(fontlist[1]);
                    curcmd.lock();
                    ImGui::Text("Current Command: %s", curcmdstr.c_str());
                    curcmd.unlock();
                    ImGui::PopFont();

                    ImGui::Separator();

                    if(ImGui::BeginChild("cmdout##serverinstall")) {
                        bufferMutex.lock();
                        ImGui::TextUnformatted(commandProgress.c_str());
                        bufferMutex.unlock();
                    }

                    ImGui::EndChild();
                }

                ImGui::End();
            }
        }


    }

    void ServerInstall::runSSHCommand(ssh_session ses, const char* command) {
        startNewCommand(command);

        ssh_channel ch = ssh_channel_new(ses);
        ssh_channel_open_session(ch);
        ssh_channel_request_exec(ch, command);

        char buffer[256];
        int nbytes;

        nbytes = ssh_channel_read(ch, buffer, sizeof(buffer), 0);
        while(nbytes > 0) {
            bufferMutex.lock();
            commandProgress.append(buffer, buffer + nbytes);
            bufferMutex.unlock();
            nbytes = ssh_channel_read(ch, buffer, sizeof(buffer), 0);
        }

        ssh_channel_send_eof(ch);
        ssh_channel_close(ch);
        ssh_channel_free(ch);
    }

    void ServerInstall::startNewCommand(const char* command) {
        curcmd.lock();
        curcmdstr = command;
        curcmd.unlock();

        bufferMutex.lock();
        commandProgress.append(SEPARATOR_STR);
        bufferMutex.unlock();
    }
}