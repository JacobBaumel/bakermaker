//
// Created by skipp on 4/27/2024.
//

#ifndef BAKERMAKER_BASEUISCREEN_H
#define BAKERMAKER_BASEUISCREEN_H

#include <map>
#include "string_theory/string"
#include "imgui.h"

namespace bakermaker {
    extern ImFont** fontlist;
    extern int latestId;
    void resetIds();

    enum class ProgramStage {
        SERVER_SETUP,
        SERVER_CONNECT,
        SSH_KEYGEN_ADMIN,
        SERVER_INSTALL,
        USER_MANAGE,
        REPO_MANAGE,

        LIBS_NOT_FOUND
    };

    class BaseUIScreen {
    public:
        explicit BaseUIScreen(ProgramStage programStage, std::map<ProgramStage, BaseUIScreen*>* map);
        virtual void render() = 0;
    };

    extern std::map<ProgramStage, BaseUIScreen*> setupScreens;
    extern std::map<ProgramStage, BaseUIScreen*> configScreens;
}

#endif //BAKERMAKER_BASEUISCREEN_H
