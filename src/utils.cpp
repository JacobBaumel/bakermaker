#include "utils.h"
#include "improgress.h"

namespace ST {
    string operator+(const string& str, int num) {
        int digits = 1;
        int numcopy = num;
        while(numcopy / 10 > 0) {
            numcopy /= 10;
            digits++;
        }

        char* charnum = new char[digits];

        for(int i = 0; i < digits; i++) {
            charnum[digits - i - 1] = '0' + (num % 10);
            num /= 10;
        }

        ST::char_buffer cat;
        cat.allocate(str.size() + digits);
        std::char_traits<char>::copy(&cat[0], str.c_str(), str.size());
        std::char_traits<char>::copy(&cat[str.size()], charnum, digits);

        delete[] charnum;
        return string::from_validated(std::move(cat));
    }
}

namespace bakermaker {
    static constexpr ImVec2 MODAL_SIZE{525, 130};
    ST::string error;
    static bool showErrorModal;

    void startErrorModal(const char* errormsg) {
        error = errormsg;
        showErrorModal = true;
    }

    void displayErrorModal() {
        if(showErrorModal) {
            ImGui::SetNextWindowSize(MODAL_SIZE);
            ImGui::SetNextWindowFocus();
            ImVec2 screenSize = ImGui::GetIO().DisplaySize;
            screenSize.x = (screenSize.x - MODAL_SIZE.x) / 2;
            screenSize.y = (screenSize.y - MODAL_SIZE.y) / 2;
            ImGui::SetNextWindowPos(screenSize);
            if(ImGui::Begin("Error Modal", &showErrorModal, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove)) {
                ImGui::Text("Encountered an error:");
                ImGui::TextUnformatted(error.c_str());
                if(ImGui::Button("Close")) showErrorModal = false;
            }

            ImGui::End();
        }
    }

    void spinner() {
        static int spinnerid = 0;
        spinnerid = (spinnerid + 1) % 1000;
        ImGui::Spinner((ST::string("##spinner") + spinnerid).c_str(), 8, 1,
                       ImGui::GetColorU32(ImVec4(0.1, 0.1, 0.75, 1)));
    }

}