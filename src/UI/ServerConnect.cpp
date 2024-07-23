#include "imgui.h"
#include "libssh/libssh.h"
#include "string_theory/string"

#include "UI/BaseUIScreen.h"
#include "UI/ServerConnect.h"
#include "ssh_helper.h"
#include "utils.h"

namespace bakermaker {
    using namespace ST::literals;

    static constexpr int FILE_PICKER_FLAGS = ImGuiFileBrowserFlags_CloseOnEsc |
        ImGuiFileBrowserFlags_ConfirmOnEnter;

    ServerConnect::ServerConnect() : BaseUIScreen(ProgramStage::SERVER_CONNECT, &setupScreens),
                                     browser(new ImGui::FileBrowser(FILE_PICKER_FLAGS)),
                                     exec(nullptr), execDone(false), success(1) {
        browser->SetTitle("Choose SSH Private Key");
        browser->ClearSelected();

        strcpy_s(ip, config["server"]["ip"].get<std::string>().c_str());
        strcpy_s(user, config["server"]["user"].get<std::string>().c_str());
        port = config["server"]["port"];

        ST::string path = config["server"]["user"];
        browser->SetPwd(path.empty() ? std::filesystem::current_path() : path.c_str());
    }

    void ServerConnect::render() {
        // Header
        ImGui::PushFont(fontlist[1]);
        ImGui::Text("Server Connection Information");
        ImGui::PopFont();
        ImGui::Separator();

        // Inputs for IP address, port, username, and ssh key path
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
            success = 1;
            execDone = false;
            exec = new std::thread([this] {
                ssh_session session;
                ST::string path = browser->GetSelected().string();
                success = createSession(session, ip, user, path.c_str(), port);
                if(success == 0) {
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
            spinner();
        }

        if(execDone) {
            if(exec) {
                exec->join();
                delete exec;
                exec = nullptr;

                switch(success) {
                case 0:
                    config["server"]["ip"] = ip;
                    config["server"]["port"] = port;
                    config["server"]["user"] = user;
                    if(std::filesystem::exists("./keyfile")) std::filesystem::remove("./keyfile");
                    std::filesystem::copy_file(browser->GetSelected().string(), "./keyfile");
                    break;

                case -1:
                    startErrorModal("Could not initialize SSH Session. Application Error. (-1)");
                    break;

                case -2:
                    startErrorModal("Could not connect to SSH server (-2)");
                    break;

                case -3:
                    startErrorModal("Error processing public key (-3)");
                    break;

                case -4:
                    startErrorModal("Error processing private key (-4)");
                    break;

                case -5:
                    startErrorModal("Could not read private key file (-5)");
                    break;

                default:
                    startErrorModal("Unknown error code!");
                }
            }

            if(success == 0) {
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0, 1, 0, 1));
                ImGui::Text("Success!");
                ImGui::PopStyleColor();
            }
        }
    }
}
