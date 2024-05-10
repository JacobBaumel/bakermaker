//
// Created by skipp on 4/27/2024.
//

#ifndef BAKERMAKER_BASEUISCREEN_H
#define BAKERMAKER_BASEUISCREEN_H

#include <map>
#include "string_theory/string"
#include "imgui.h"

namespace ST {
    string operator+(const string& str, int num);
}

namespace bakermaker {
    extern ImFont** fontlist;
    extern int latestId;
    const char* appendId(ST::string string);
    const char* appendId(const char*);
    void resetIds();

    enum class ProgramStage {
        SERVER_SETUP,
        SERVER_CONNECT,
        SERVER_INSTALL,
        SSH_KEYGEN,

        LIBS_NOT_FOUND
    };

    class BaseUIScreen {
    public:
        explicit BaseUIScreen(ProgramStage programStage);
        virtual void render() = 0;
    };

    extern std::map<ProgramStage, BaseUIScreen*> screens;
}

#endif //BAKERMAKER_BASEUISCREEN_H
