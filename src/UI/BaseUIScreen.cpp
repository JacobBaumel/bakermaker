#include "UI/BaseUIScreen.h"

namespace bakermaker {
    ImFont** fontlist;
    int latestId;
    std::map<ProgramStage, BaseUIScreen*> screens;

    void resetIds() {
        latestId = 0;
    }

    BaseUIScreen::BaseUIScreen(ProgramStage programStage) {
        screens[programStage] = this;
    }
}