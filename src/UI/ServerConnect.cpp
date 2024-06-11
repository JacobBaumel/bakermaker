#include "imgui.h"
#include "UI/ServerConnect.h"
#include "UI/BaseUIScreen.h"
#include <iostream>
#include "utils.h"
#include "string_theory/string"

namespace bakermaker {
    using namespace ST::literals;

    ServerConnect::ServerConnect() : BaseUIScreen(bakermaker::ProgramStage::SERVER_CONNECT),
                                     execDone(false) {
        browser = new ImGui::FileBrowser(FILE_PICKER_FLAGS);
        browser->SetTitle("Choose SSH Private Key");
        browser->ClearSelected();

        strcpy_s(ip, config["server"]["ip"].get<std::string>().c_str());
        strcpy_s(user, config["server"]["user"].get<std::string>().c_str());
        port = config["server"]["port"];

        ST::string path = config["server"]["user"];
        browser->SetPwd(path.empty() ? std::filesystem::current_path() : path.c_str());
    }

    void ServerConnect::render() {
        ImGui::PushFont(fontlist[1]);
        ImGui::Text("Server Connection Information");
        ImGui::PopFont();
        ImGui::Separator();

        ImGui::Text("Server IP: ");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(150);
        ImGui::InputText("##ip_enter", ip, 16);

        ImGui::Text("Username: ");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(150);
        ImGui::InputText("##user_enter", user, 33);

        ImGui::Text("Server Port: ");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(100);
        ImGui::InputInt("##port_enter", &port);
        if(port < 0) port = 0;
        if(port > 65535) port = 65535;

        browser->Display();
        ImGui::NewLine();
        if(ImGui::Button("Browse for server private key")) browser->Open();
        if(browser->HasSelected()) {
            ImGui::Text("Selected File: %s", browser->GetSelected().string().c_str());
        }

        if(exec && !execDone) ImGui::BeginDisabled();

        ImGui::NewLine();
        if(ImGui::Button("Submit##server_connect")) {
            sshrc = 1;
            execDone = false;
            exec = new std::thread([this](){
                ssh_session session;
                ST::string path = browser->GetSelected().string();
                sshrc = createSession(session, ip, user, path.c_str(), port);
                if(sshrc == 0) {
                    ssh_disconnect(session);
                    ssh_free(session);
                }
                execDone = true;
            });
            ImGui::BeginDisabled();
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

                config["server"]["ip"] = ip;
                config["server"]["port"] = port;
                config["server"]["user"] = user;
                config["server"]["keyfile"] = browser->GetSelected().string();

                switch(sshrc) {
                    case -1:
                        bakermaker::startErrorModal("Could not initialize SSH Session. Application Error. (-1)");
                        break;

                    case -2:
                        bakermaker::startErrorModal("Could not connect to SSH server (-2)");
                        break;

                    case -3:
                        bakermaker::startErrorModal("Error processing public key (-3)");
                        break;

                    case -4:
                        bakermaker::startErrorModal("Error processing private key (-4)");
                        break;

                    case -5:
                        bakermaker::startErrorModal("Could not read private key file (-5)");
                        break;
                }
            }

            if(sshrc == 0) {
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0, 1, 0, 1));
                ImGui::Text("Success!");
                ImGui::PopStyleColor();
            }
        }
    }
}
