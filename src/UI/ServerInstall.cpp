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

                uploadToRemote(sftp, (ST::string("keys/") +
                                      (config["keys"][0].get<std::string>() + ".pub")).c_str(), "authorized_keys");

                uploadToRemote(sftp, (ST::string("keys/") + config["keys"][0].get<std::string>()).c_str(), "gito");

                runSSHCommand(ubuntu, "sudo mkdir /home/git/.ssh; echo 'Created .ssh folder'");
                runSSHCommand(ubuntu, "sudo cp /home/ubuntu/authorized_keys "
                                      "/home/git/.ssh/authorized_keys; echo 'Moved authorized_keys file'");
                runSSHCommand(ubuntu, "sudo chown -R git:git /home/git/.ssh; echo 'Changed .ssh ownership'");
                runSSHCommand(ubuntu, "sudo chown -R git:git /home/git/.ssh/authorized_keys;"
                                      " echo 'Changed authorized_keys ownership'");
                runSSHCommand(ubuntu, "sudo chmod 700 /home/git/.ssh; echo 'Changed .ssh folder perms'");
                runSSHCommand(ubuntu, "sudo chmod 644 /home/git/.ssh/authorized_keys; echo 'Changed keys perms'");
                runSSHCommand(ubuntu, "mkdir -p .ssh");
                runSSHCommand(ubuntu, "mv ./authorized_keys .ssh/gito");
                runSSHCommand(ubuntu, (ST::string("echo 'Host gito\n\tHostName 127.0.0.1") +
                                       "\n\tUser git\n\tIdentityFile ~/.ssh/gito' > .ssh/config").c_str());

                ssh_session git;
                {
                    int rc = bakermaker::createSession(git, config["server"]["ip"].get<std::string>().c_str(),
                                                       "git", (ST::string("keys/") +
                                                               config["keys"][0].get<std::string>()).c_str());

                    if(rc != SSH_OK) return;
                }

                runSSHCommand(git, "mkdir /home/git/bin");
                runSSHCommand(git, (ST::string("mv .ssh/authorized_keys ./") +
                                    config["keys"][0].get<std::string>() + ".p").c_str());
                runSSHCommand(git, "git clone https://github.com/sitaramc/gitolite");
                runSSHCommand(git, "git config --global --add safe.directory /home/git/gitolite");
                runSSHCommand(git, "gitolite/install -ln /home/git/bin");
                runSSHCommand(git, (ST::string("bin/gitolite setup -pk ") + config["keys"][0].get<std::string>()).c_str());

                runSSHCommand(ubuntu, "git clone gito:gitolite-admin");

                curcmd.lock();
                curcmdstr = "Finished";
                curcmd.unlock();

                bufferMutex.lock();
                commandProgress.append(SEPARATOR_STR);
                bufferMutex.unlock();

                ssh_disconnect(ubuntu);
                ssh_free(ubuntu);
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