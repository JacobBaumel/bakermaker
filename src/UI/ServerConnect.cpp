#include "UI/ServerConnect.h"

#include "imgui.h"
#include "setup.h"

namespace bakermaker {
    void ServerConnect::render(bakermaker::ProgramStage& stage) {
        ImGui::Text("Enter in server connection information:");
        ImGui::Text("Server IP: ");
        ImGui::SameLine();
        ImGui::InputText(appendId("##ip_enter"), ip, 16);
    }
}
