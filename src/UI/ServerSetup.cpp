#include "UI/ServerSetup.h"

namespace bakermaker {
    ServerSetup::ServerSetup() : BaseUIScreen(bakermaker::ProgramStage::SERVER_SETUP) {}

    void ServerSetup::render(bakermaker::ProgramStage &stage) {
        ImGui::PushFont(fontlist[1]);
        ImGui::Text("Server Setup");
        ImGui::PopFont();
        ImGui::Separator();

        ImGui::Text("iSCSI Command 1: ");
        ImGui::SameLine();
        ImGui::InputText("##iscsi_1", e, 2);
        ImGui::NewLine();

        ImGui::Text("iSCSI Command 2: ");
        ImGui::SameLine();
        ImGui::InputText("##iscsi_2", e, 2);
        ImGui::NewLine();

        ImGui::Text("iSCSI Command 3: ");
        ImGui::SameLine();
        ImGui::InputText("##iscsi_3", e, 2);
        ImGui::NewLine();

        ImGui::Button("Submit##server_setup");
    }
}