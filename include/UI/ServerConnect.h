//
// Created by skipp on 4/28/2024.
//

#ifndef TEST_MD_SERVERCONNECT_H
#define TEST_MD_SERVERCONNECT_H

#include "UI/BaseUIScreen.h"
#include "imfilebrowser.h"
#include <atomic>
#include <thread>
#include "ssh_helper.h"

namespace bakermaker {
    class ServerConnect : public BaseUIScreen {

    private:
        static constexpr int FILE_PICKER_FLAGS = ImGuiFileBrowserFlags_CloseOnEsc |
                ImGuiFileBrowserFlags_ConfirmOnEnter;

        char ip[16]{};
        char user[33]{};
        int port = 22;
        ImGui::FileBrowser* browser = nullptr;
        std::thread* exec = nullptr;
        std::atomic_bool execDone;
        int sshrc = 1;


    public:
        ServerConnect();
        void render() override;
    };
}

#endif //TEST_MD_SERVERCONNECT_H
