//
// Created by skipp on 4/28/2024.
//

#ifndef TEST_MD_SERVERSETUP_H
#define TEST_MD_SERVERSETUP_H

#include "UI/BaseUIScreen.h"

namespace bakermaker {
    class ServerSetup : public BaseUIScreen {
        char e[2] {' ', '\0'};

    public:
        ServerSetup();
        void render(bakermaker::ProgramStage& stage) override;
    };
}

#endif //TEST_MD_SERVERSETUP_H
