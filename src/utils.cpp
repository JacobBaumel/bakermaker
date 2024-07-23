#include "improgress.h"
#include "utils.h"

namespace bakermaker {
    ST::string error;
    static bool showErrorModal;

    void startErrorModal(const char* errormsg) {
        error = errormsg;
        showErrorModal = true;
    }

    void displayErrorModal() {
        if(showErrorModal) {
            ImVec2 size = ImVec2(525, 100 + (error.size() / 75) * 15);
            ImGui::SetNextWindowSize(size);
            ImGui::SetNextWindowFocus();
            ImVec2 screenSize = ImGui::GetIO().DisplaySize;
            screenSize.x = (screenSize.x - size.x) / 2;
            screenSize.y = (screenSize.y - size.y) / 2;
            ImGui::SetNextWindowPos(screenSize);
            if(ImGui::Begin("Error Modal", &showErrorModal, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove)) {
                ImGui::Text("Encountered an error:");
                ImGui::TextWrapped("%s", error.c_str());
                if(ImGui::Button("Close")) showErrorModal = false;
            }

            ImGui::End();
        }
    }

    void spinner() {
        static int spinnerid = 0;
        spinnerid = (spinnerid + 1) % 1000;
        ImGui::Spinner((ST::string("##spinner") + std::to_string(spinnerid)).c_str(), 8, 1,
                       ImGui::GetColorU32(ImVec4(0.9, 0.9, 0.9, 1)));
    }
}