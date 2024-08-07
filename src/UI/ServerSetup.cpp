#include "UI/ServerSetup.h"
#include "utils.h"

namespace bakermaker {
    ServerSetup::ServerSetup() : BaseUIScreen(ProgramStage::SERVER_SETUP, &setupScreens), useiscsi(true),
                                 submitted(false) {
        strcpy_s(c1, config["iscsi"][0].get<std::string>().c_str());
        strcpy_s(c2, config["iscsi"][1].get<std::string>().c_str());
        strcpy_s(c3, config["iscsi"][2].get<std::string>().c_str());
        useiscsi = config["useiscsi"].get<bool>();
    }

    void ServerSetup::render() {
        // Header
        ImGui::PushFont(fontlist[1]);
        ImGui::Text("Server Setup");
        ImGui::PopFont();
        ImGui::Separator();

        // Checkbox to enable or disable using iscsi
        ImGui::Text("Use iSCSI Storage");
        ImGui::SameLine();
        if(ImGui::Checkbox("##useiscsi", &useiscsi)) config["useiscsi"] = useiscsi;

        // Input fields for the 3 iscsi commands
        if(!useiscsi) ImGui::BeginDisabled();
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
            submitted = true;
        }

        if(submitted) {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0, 1, 0, 1));
            ImGui::Text("Success!");
            ImGui::PopStyleColor();
        }

        if(!useiscsi) ImGui::EndDisabled();
    }
}
