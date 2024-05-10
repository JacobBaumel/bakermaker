#include "imgui.h"
#include "UI/ServerConnect.h"
#include "UI/BaseUIScreen.h"
#include <iostream>
#include "improgress.h"
#include "utils.h"
#include "string_theory/string"

namespace bakermaker {
    using namespace ST::literals;

    ServerConnect::ServerConnect() : BaseUIScreen(bakermaker::ProgramStage::SERVER_CONNECT),
        connectDone(false) {
        browser = new ImGui::FileBrowser(FILE_PICKER_FLAGS);
        browser->SetTitle("Choose SSH Private Key");
        browser->ClearSelected();

        strcpy_s(ip, std::string(config["server"]["ip"]).c_str());
        strcpy_s(user, std::string(config["server"]["user"]).c_str());
        port = int(config["server"]["port"]);

        ST::string path = std::string(config["server"]["user"]);
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

        ImGui::NewLine();
        if(ImGui::Button("Submit##server_connect")) {
            showModal = true;
            sshrc = 1;
            connectDone = false;
            connectThread = new std::thread([this](){
                ssh_session session;
                ST::string path = browser->GetSelected().string();
                sshrc = createSession(session, ip, user, path.c_str(), port);
                if(sshrc == 0) {
                    ssh_disconnect(session);
                    ssh_free(session);
                }
                connectDone = true;
            });
        }

        if(showModal) {
            ImGui::SetNextWindowFocus();
            ImGui::SetNextWindowSize(MODAL_SIZE);
            ImVec2 screenSize = ImGui::GetIO().DisplaySize;
            screenSize.x = (screenSize.x - MODAL_SIZE.x) / 2;
            screenSize.y = (screenSize.y - MODAL_SIZE.y) / 2;
            ImGui::SetNextWindowPos(screenSize);
            if(ImGui::Begin("Test Modal", &showModal, modalFlags)) {
                ImGui::NewLine();
                ImGui::Text("Testing connectivity to remote server ...");
                if(sshrc == 1) {
                    ImGui::SameLine(0, 15);
                    ImGui::Spinner("##connect_load", 10, 1,
                                   ImGui::GetColorU32(ImVec4(0.1, 0.1, 0.75, 1)));
                }

                if(connectDone) {
                    if(connectThread != nullptr) {
                        connectThread->join();
                        delete connectThread;
                        connectThread = nullptr;

                        config["server"]["ip"] = ip;
                        config["server"]["port"] = port;
                        config["server"]["user"] = user;
                        config["server"]["keyfile"] = browser->GetSelected().string();
                    }

                    ImGui::PushStyleColor(ImGuiCol_Text, sshrc == 0 ?
                        ImVec4(0, 1, 0, 1) : ImVec4(1, 0, 0, 1));

                    switch(sshrc) {
                        case 0:
                            ImGui::Text("Success!");
                            break;

                        case -1:
                            ImGui::Text("Could not initialize SSH Session. Application Error. (-1)");
                            break;

                        case -2:
                            ImGui::Text("Could not connect to SSH server (-2)");
                            break;

                        case -3:
                            ImGui::Text("Error processing public key (-3)");
                            break;

                        case -4:
                            ImGui::Text("Error processing private key (-4)");
                            break;

                        case -5:
                            ImGui::Text("Could not read private key file (-5)");
                            break;
                    }

                    ImGui::PopStyleColor(1);

                    if(ImGui::Button("OK")) {
                        showModal = false;
                    }
                }
            }

            ImGui::End();
        }
    }
}
