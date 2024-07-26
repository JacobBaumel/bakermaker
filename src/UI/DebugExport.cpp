#include "UI/DebugExport.h"
#include "unzip_utils.h"
#include "utils.h"

namespace bakermaker {
    DebugExport::DebugExport() : BaseUIScreen(ProgramStage::DEBUG_EXPORT, &configScreens), showSuccess(false) {
    }

    // Very simple section. Just show button to create the export. It is a fairly quick function anyways, and its better
    // to lock the user out of editing things completely while the export is creating.
    void DebugExport::render() {
        ImGui::PushFont(fontlist[1]);
        ImGui::Text("Create Debug Export");
        ImGui::PopFont();
        ImGui::Separator();

        ImGui::Text("Create zip archive of all important data.");
        if(ImGui::Button("Create##debug_export_create")) {
            showSuccess = false;
            if(!zip_debug()) {
                startErrorModal("Failed to create debug export!");
            }
            else showSuccess = true;
        }

        if(showSuccess) {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0, 1, 0, 1));
            ImGui::Text("Success!");
            ImGui::PopStyleColor();
        }
    }
}
