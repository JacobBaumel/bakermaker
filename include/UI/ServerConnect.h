//
// Created by skipp on 4/28/2024.
//

#ifndef TEST_MD_SERVERCONNECT_H
#define TEST_MD_SERVERCONNECT_H

#include "UI/BaseUIScreen.h"
#include "imfilebrowser.h"

namespace bakermaker {
    class ServerConnect : public BaseUIScreen {

    private:
        char ip[16];
        char user[33];
        int port;
        ImGui::FileBrowser* browser;


    public:
        void render(bakermaker::ProgramStage& stage) override;
    };
}

#endif //TEST_MD_SERVERCONNECT_H
