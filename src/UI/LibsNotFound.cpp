#include "romfs/romfs.hpp"

#include "UI/BaseUIScreen.h"
#include "UI/LibsNotFound.h"
#include "unzip_utils.h"
#include "utils.h"

namespace bakermaker {
    static constexpr ImVec2 MODAL_SIZE{525, 130};
    static constexpr ImGuiWindowFlags modalFlags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_Modal;

    LibsNotFound::LibsNotFound() : BaseUIScreen(ProgramStage::LIBS_NOT_FOUND, nullptr) {
        const romfs::Resource libs = romfs::get("libs.zip");
        unzip_from_mem(libs.data(), libs.size());
    }

    // Extraction is just done in the constructor. No real need for threading because this extraction is very quick
    void LibsNotFound::render() {
        ImGui::SetNextWindowFocus();
        ImGui::SetNextWindowSize(MODAL_SIZE);
        ImVec2 screenSize = ImGui::GetIO().DisplaySize;
        screenSize.x = (screenSize.x - MODAL_SIZE.x) / 2;
        screenSize.y = (screenSize.y - MODAL_SIZE.y) / 2;
        ImGui::SetNextWindowPos(screenSize);
        if(ImGui::Begin("Test Modal", nullptr, modalFlags)) {
            ImGui::Text("Extracting Libraries");
            ImGui::Text("Library Extraction Complete. Please restart the program.");
        }

        ImGui::End();
    }
}
