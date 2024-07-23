#include "UI/ServerInstall.h"

#include <imgui_internal.h>

#include "romfs/romfs.hpp"
#include "utils.h"
#include "libssh/libssh.h"
#include "libssh/sftp.h"
#include "ssh_helper.h"

#include <iostream>

namespace bakermaker {
    ServerInstall::ServerInstall()
            :
            BaseUIScreen(bakermaker::ProgramStage::SERVER_INSTALL, &bakermaker::setupScreens),
            execDone(false), success(0), bufferMutex(),
            showCommandOutputs(false), exec(nullptr) {
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

        if(config["keys"].empty() || config["server"]["ip"].get<ST::string>().empty() || (exec && !execDone) || (
            execDone && success == 0))
            ImGui::BeginDisabled();

        if(ImGui::Button("Begin##server_install_begin")) {
            if(config["useiscsi"].get<bool>() && (config["iscsi"][0].get<std::string>().empty() ||
                                                  config["iscsi"][1].get<std::string>().empty() ||
                                                  config["iscsi"][2].get<std::string>().empty())) {
                bakermaker::startErrorModal("ISCSI is enabled, but not all fields are filled in.");
            }

            else {
                ImGui::BeginDisabled();

                execDone = false;
                success = 0;
                bool useiscsi = config["useiscsi"].get<bool>();
                InstallParams ip{config["server"]["ip"].get<std::string>(),
                                 config["server"]["port"].get<int>(),
                                 config["server"]["user"].get<std::string>(),
                                 "./keyfile",
                                 config["keys"][0].get<std::string>(),
                                 useiscsi,
                                 useiscsi ? '"' + config["iscsi"][0].get<std::string>() + "\" "
                                          : "",
                                 useiscsi ? '"' + config["iscsi"][1].get<std::string>() + "\" "
                                          : "",
                                 useiscsi ? '"' + config["iscsi"][2].get<std::string>() + "\" "
                                          : ""};
                exec = new std::thread(&ServerInstall::install, this, ip);
            }
        }

        if(config["keys"].empty()) {
            if(ImGui::GetCurrentContext()->DisabledStackSize > 0) ImGui::EndDisabled();
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 0, 0, 1));
            ImGui::Text("Create admin key first!");
            ImGui::PopStyleColor();
        }

        if(config["server"]["ip"].get<ST::string>().empty()) {
            if(ImGui::GetCurrentContext()->DisabledStackSize > 0) ImGui::EndDisabled();
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 0, 0, 1));
            ImGui::Text("Verify server connection info first!");
            ImGui::PopStyleColor();
        }

        if(exec && !execDone) {
            ImGui::EndDisabled();
            ImGui::SameLine();
            bakermaker::spinner();
        }

        if(execDone) {
            if(exec) {
                exec->join();
                delete exec;
                exec = nullptr;

                if(success == -1) bakermaker::startErrorModal("Failed when connecting to server!");
                else if(success == 1)
                    bakermaker::startErrorModal("Failed when installing components. "
                                                "Read log to find issue. Press \"Begin\" again to resume at last failed commmand.");
            }

            if(success == 0) {
                ImGui::EndDisabled();

                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0, 1, 0, 1));
                ImGui::Text("Installation Successful!");
                ImGui::PopStyleColor();

                ImGui::SameLine();

                if(ImGui::Button("Ok##installok")) {
                    config["setup"] = true;
                }
            }
        }

        if(exec && ImGui::Button("Show/Hide Command Outputs"))
            showCommandOutputs = !showCommandOutputs;

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

                ImGui::Separator();
                if(ImGui::Button("Close##command_screen")) {
                    showCommandOutputs = false;
                }

                ImGui::EndChild();
            }

            ImGui::End();
        }
    }

    int ServerInstall::runSSHCommand(ssh_session ses, const char* command) {
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

        int rc = ssh_channel_get_exit_status(ch);

        ssh_channel_send_eof(ch);
        ssh_channel_close(ch);
        ssh_channel_free(ch);

        return rc;
    }

    void ServerInstall::startNewCommand(const char* command) {
        curcmd.lock();
        curcmdstr = "Installing";
        curcmd.unlock();

        bufferMutex.lock();
        commandProgress.append(SEPARATOR_STR);
        bufferMutex.unlock();
    }

    void ServerInstall::install(const bakermaker::ServerInstall::InstallParams& ip) {
        ssh_session ubuntu;
        memset((void*) &ubuntu, 0, sizeof(ssh_session));
        {
            int rc = bakermaker::createSession(ubuntu, ip.ip.c_str(), ip.user.c_str(),
                                               ip.keyfile.c_str(), ip.port);

            if(rc != SSH_OK) {
                success = -1;
                execDone = true;
                return;
            }
        }

        sftp_session sftp;

        if(runSSHCommand(ubuntu, "test -f progress") == 1) {
            sftp = sftp_new(ubuntu);
            sftp_init(sftp);

            uploadToRemote(sftp, (ST::string("keys/") + ip.adminkey).c_str(),
                           "gito");

            uploadToRemote(sftp, (ST::string("keys/") + ip.adminkey + ".pub").c_str(),
                           "authorized_keys");

            {
                romfs::Resource script = romfs::get("install.sh");
                uploadToRemote(sftp, (void*) script.data(), script.size(), "install.sh");
                script = romfs::get("commitall.sh");
                uploadToRemote(sftp, (void*) script.data(), script.size(), "commitall.sh");
                script = romfs::get("gituserinstall.sh");
                uploadToRemote(sftp, (void*) script.data(), script.size(), "gituserinstall.sh");
            }

            sftp_free(sftp);

            runSSHCommand(ubuntu, "chmod +x ~/install.sh");
            runSSHCommand(ubuntu, "chmod +x ~/commitall.sh");
        }

        success = runSSHCommand(ubuntu, (ST::string("~/install.sh ") + (ip.useiscsi ? "1 " : "0 ") +
                                         ip.i1 + ip.i2 + ip.i3).c_str());

        curcmd.lock();
        curcmdstr = "Finished";
        curcmd.unlock();

        bufferMutex.lock();
        commandProgress.append(SEPARATOR_STR);
        bufferMutex.unlock();

        ssh_disconnect(ubuntu);
        ssh_free(ubuntu);

        execDone = true;
    }
}