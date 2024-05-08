#include "UI/ServerInstall.h"
#include "romfs/romfs.hpp"
#include "setup.h"
#include "libssh/libssh.h"
#include "ssh_helper.h"

#include "UI/InstallCommands.inc"

#include <iostream>

namespace bakermaker {
    ServerInstall::ServerInstall() : BaseUIScreen(bakermaker::ProgramStage::SERVER_INSTALL),
        execDone(false), execProgress(0), bufferMutex(), hasStartedExec(false), showCommandOutputs(false) {
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
        if(ImGui::Button("Begin##server_install_begin")) {
            hasStartedExec = true;

            exec = new std::thread([this](){
                ssh_session ses;
                memset((void*) &ses, 0, sizeof(ssh_session));
                bakermaker::createSession(ses, std::string(config["server"]["ip"]).c_str(),
                                          std::string(config["server"]["user"]).c_str(),
                                          std::string(config["server"]["keyfile"]).c_str(), 0);

                ssh_channel ch;
                char buffer[256];
                int nbytes = 0;

                for(const auto& cmd : bakermaker::commands) {
                    curcmd.lock();
                    curcmdstr = cmd;
                    curcmd.unlock();

                    bufferMutex.lock();
                    commandProgress.append(SEPARATOR_STR);
                    bufferMutex.unlock();

                    ch = ssh_channel_new(ses);
                    ssh_channel_open_session(ch);
                    ssh_channel_request_exec(ch, cmd);

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

                curcmd.lock();
                curcmdstr = "Finished";
                curcmd.unlock();

                bufferMutex.lock();
                commandProgress.append(SEPARATOR_STR);
                bufferMutex.unlock();

                ssh_disconnect(ses);
                ssh_free(ses);
            });
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
}