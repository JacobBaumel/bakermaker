#include "UI/BaseUIScreen.h"

namespace bakermaker {
    ImFont** fontlist;
    int latestId;
    std::map<ProgramStage, BaseUIScreen*> setupScreens;
    std::map<ProgramStage, BaseUIScreen*> configScreens;

    void resetIds() {
        latestId = 0;
    }

    BaseUIScreen::BaseUIScreen(ProgramStage programStage, std::map<ProgramStage, BaseUIScreen*>* map) {
        if(map == nullptr) return;
        (*map)[programStage] = this;
    }
}