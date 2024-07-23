#include <fstream>

#include "imgui_internal.h"
#include "libssh/libssh.h"
#include "libssh/sftp.h"
#include "romfs/romfs.hpp"

#include "UI/ServerInstall.h"
#include "setup.h"
#include "ssh_helper.h"
#include "utils.h"

namespace bakermaker {
    using namespace ST::literals;
    static constexpr char SEPARATOR_STR[63] = "\n\n---------------------------------------------------------\n\n\0";

    ServerInstall::ServerInstall()
        : BaseUIScreen(ProgramStage::SERVER_INSTALL, &setupScreens), instructions(""_st), exec(nullptr), success(0),
          execDone(false), showCommandOutputs(false) {
        const romfs::Resource md = romfs::get("ServerInstallText.md");
        instructions = ST::string((const char*)md.data(), md.size());
    }

    void ServerInstall::render() {
        ImGui::PushFont(fontlist[1]);
        ImGui::Text("Server Software Install");
        ImGui::PopFont();
        ImGui::Separator();

        documentation->render(instructions);

        ImGui::NewLine();

        if(config["keys"].empty() || config["server"]["ip"].get<ST::string>().empty() || (exec && !execDone) || (
            execDone && success == 0))
            ImGui::BeginDisabled();

        if(ImGui::Button("Begin##server_install_begin")) {
            // Check that if iscsi is enabled, all 3 commands are present
            if(config["useiscsi"].get<bool>() && (config["iscsi"][0].get<std::string>().empty() ||
                config["iscsi"][1].get<std::string>().empty() ||
                config["iscsi"][2].get<std::string>().empty())) {
                startErrorModal("ISCSI is enabled, but not all fields are filled in.");
            }

            else {
                ImGui::BeginDisabled();

                execDone = false;
                success = 0;
                bool useiscsi = config["useiscsi"].get<bool>();

                // Prepare the parameters to the thread
                InstallParams ip{
                    config["server"]["ip"].get<std::string>(),
                    config["server"]["port"].get<int>(),
                    config["server"]["user"].get<std::string>(),
                    "./keyfile",
                    config["keys"][0].get<std::string>(),
                    useiscsi,
                    useiscsi
                        ? '"' + config["iscsi"][0].get<std::string>() + "\" "
                        : "",
                    useiscsi
                        ? '"' + config["iscsi"][1].get<std::string>() + "\" "
                        : "",
                    useiscsi
                        ? '"' + config["iscsi"][2].get<std::string>() + "\" "
                        : ""
                };
                exec = new std::thread(&ServerInstall::install, this, ip);
            }
        }

        // Check that both the admin key has been created, and server connectivity verified
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
            spinner();
        }

        if(execDone) {
            if(exec) {
                exec->join();
                delete exec;
                exec = nullptr;

                if(success == -1) startErrorModal("Failed when connecting to server!");
                else if(success == 1)
                    startErrorModal("Failed when installing components. "
                        "Read install.log to find issue. Press \"Begin\" again to resume at last failed commmand.");
                std::ofstream installlog("install.log", std::ios::trunc);
                installlog << commandProgress.c_str();
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

        // Modal to show the messages from the install
        if(showCommandOutputs) {
            ImGui::SetNextWindowSize(ImVec2(500, 500), ImGuiCond_FirstUseEver);
            if(ImGui::Begin("Command Output")) {
                ImGui::PushFont(fontlist[1]);
                ImGui::Text(execDone ? "Finished" : "Installing");
                ImGui::PopFont();

                ImGui::Separator();

                if(ImGui::BeginChild("cmdout##serverinstall", ImVec2(0, ImGui::GetWindowHeight() - 100))) {
                    bufferMutex.lock();
                    ImGui::TextUnformatted(commandProgress.c_str());
                    bufferMutex.unlock();
                    ImGui::EndChild();
                }

                ImGui::Separator();
                if(ImGui::Button("Close##command_screen")) {
                    showCommandOutputs = false;
                }
            }

            ImGui::End();
        }
    }

    int ServerInstall::runSSHCommand(ssh_session ses, const char* command) {
        bufferMutex.lock();
        commandProgress.append(SEPARATOR_STR);
        bufferMutex.unlock();

        ssh_channel ch = ssh_channel_new(ses);
        ssh_channel_open_session(ch);
        ssh_channel_request_exec(ch, command);

        char buffer[256];

        int nbytes = ssh_channel_read(ch, buffer, sizeof(buffer), 0);
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

    void ServerInstall::install(const InstallParams& ip) {
        // Create the ssh session
        ssh_session ubuntu = {};
        {
            int rc = createSession(ubuntu, ip.ip.c_str(), ip.user.c_str(),
                                               ip.keyfile.c_str(), ip.port);

            if(rc != SSH_OK) {
                success = -1;
                execDone = true;
                return;
            }
        }

        if(runSSHCommand(ubuntu, "test -f progress") == 1) {
            sftp_session sftp = sftp_new(ubuntu);
            sftp_init(sftp);

            uploadToRemote(sftp, ("keys/"_st + ip.adminkey).c_str(),
                           "gito");

            uploadToRemote(sftp, ("keys/"_st + ip.adminkey + ".pub").c_str(),
                           "authorized_keys");

            {
                romfs::Resource script = romfs::get("install.sh");
                uploadToRemote(sftp, (void*)script.data(), script.size(), "install.sh");
                script = romfs::get("commitall.sh");
                uploadToRemote(sftp, (void*)script.data(), script.size(), "commitall.sh");
                script = romfs::get("gituserinstall.sh");
                uploadToRemote(sftp, (void*)script.data(), script.size(), "gituserinstall.sh");
            }

            sftp_free(sftp);

            runSSHCommand(ubuntu, "chmod +x ~/install.sh");
            runSSHCommand(ubuntu, "chmod +x ~/commitall.sh");
        }

        success = runSSHCommand(ubuntu, ("~/install.sh "_st + (ip.useiscsi ? "1 " : "0 ") +
                                    ip.i1 + ip.i2 + ip.i3).c_str());

        bufferMutex.lock();
        commandProgress.append(SEPARATOR_STR);
        bufferMutex.unlock();

        ssh_disconnect(ubuntu);
        ssh_free(ubuntu);

        execDone = true;
    }
}
