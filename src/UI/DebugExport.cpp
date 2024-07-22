#include "UI/DebugExport.h"
#include "unzip_utils.h"

namespace bakermaker {
    DebugExport::DebugExport() : BaseUIScreen(ProgramStage::DEBUG_EXPORT, &configScreens) {
    }

    void DebugExport::render() {
        ImGui::PushFont(fontlist[1]);
        ImGui::Text("Create Debug Export");
        ImGui::PopFont();
        ImGui::Text("Create zip archive of all important data.");
        if(ImGui::Button("Create")) {
            zip_debug();
        }
    }


}
