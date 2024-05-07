#include "UI/ServerSetup.h"
#include "setup.h"

namespace bakermaker {
    ServerSetup::ServerSetup() : BaseUIScreen(bakermaker::ProgramStage::SERVER_SETUP) {
        strcpy_s(c1, std::string(config["iscsi"][0]).c_str());
        strcpy_s(c2, std::string(config["iscsi"][1]).c_str());
        strcpy_s(c3, std::string(config["iscsi"][2]).c_str());
    }

    void ServerSetup::render() {
        ImGui::PushFont(fontlist[1]);
        ImGui::Text("Server Setup");
        ImGui::PopFont();
        ImGui::Separator();

        ImGui::Text("iSCSI Command 1: ");
        ImGui::SameLine();
        ImGui::InputText("##iscsi_1", c1, BUFFER_LENGTH);
        ImGui::NewLine();

        ImGui::Text("iSCSI Command 2: ");
        ImGui::SameLine();
        ImGui::InputText("##iscsi_2", c2, BUFFER_LENGTH);
        ImGui::NewLine();

        ImGui::Text("iSCSI Command 3: ");
        ImGui::SameLine();
        ImGui::InputText("##iscsi_3", c3, BUFFER_LENGTH);
        ImGui::NewLine();

        if(ImGui::Button("Submit##server_setup")) {
            config["iscsi"] = {c1, c2, c3};
        }
    }
}