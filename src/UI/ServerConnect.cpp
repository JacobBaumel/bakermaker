#include "imgui.h"
#include "UI/ServerConnect.h"
#include "UI/BaseUIScreen.h"
#include <iostream>

namespace bakermaker {
    using namespace ST::literals;

    ServerConnect::ServerConnect() : BaseUIScreen(bakermaker::ProgramStage::SERVER_CONNECT) {
        browser = new ImGui::FileBrowser(FILE_PICKER_FLAGS);
        browser->SetTitle("Choose SSH Private Key");
    }

    void ServerConnect::render(bakermaker::ProgramStage& stage) {
        ImGui::PushFont(fontlist[1]);
        ImGui::Text("Server Connection Information");
        ImGui::PopFont();
        ImGui::Separator();

        ImGui::Text("Server IP: ");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(150);
        ImGui::InputText("##ip_enter", ip, 16);

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
            ST::string words = "Selected File: "_st + browser->GetSelected().c_str();
            ImGui::Text(words.c_str());
        }
    }
}
