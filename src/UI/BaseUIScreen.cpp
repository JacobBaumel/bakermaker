#include "UI/BaseUIScreen.h"

namespace bakermaker {
    ImFont** fontlist;
    int latestId;
    std::map<ProgramStage, BaseUIScreen*> screens;

    const char* appendId(::ST::string string) {
        return (string + latestId).c_str();
    }

    const char* appendId(const char* str) {
        return appendId(ST::string(str));
    }

    void resetIds() {
        latestId = 0;
    }

    BaseUIScreen::BaseUIScreen(ProgramStage programStage) {
        screens[programStage] = this;
    }
}