//
// Created by skipp on 4/27/2024.
//

#ifndef BAKERMAKER_BASEUISCREEN_H
#define BAKERMAKER_BASEUISCREEN_H

#include <map>
#include "string_theory/string"

namespace ST {
    string operator+(const string& str, int num);
}

namespace bakermaker {
    static int latestId;
    static const char* appendId(ST::string string);
    static const char* appendId(const char*);
    static void resetIds();

    enum class ProgramStage {
        SERVER_CONNECT,
        SERVER_SETUP
    };

    class BaseUIScreen {
    public:

        BaseUIScreen(ProgramStage programStage);
        virtual void render(ProgramStage& stage) = 0;
    };

    static std::map<ProgramStage, BaseUIScreen*> screens;
}

#endif //BAKERMAKER_BASEUISCREEN_H
