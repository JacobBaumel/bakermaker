//
// Created by skipp on 4/30/2024.
//

#ifndef BAKERMAKER_SERVERINSTALL_H
#define BAKERMAKER_SERVERINSTALL_H

#include "UI/BaseUIScreen.h"

namespace bakermaker {
    class ServerInstall : public BaseUIScreen {
        char e[1] = {' '};
        ST::string instructions;

    public:
        ServerInstall();
        void render(bakermaker::ProgramStage& stage) override;
    };
}

#endif //BAKERMAKER_SERVERINSTALL_H
