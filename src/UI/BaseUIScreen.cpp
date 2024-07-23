#include "UI/BaseUIScreen.h"

namespace bakermaker {
    ImFont** fontlist;
    std::map<ProgramStage, BaseUIScreen*> setupScreens;
    std::map<ProgramStage, BaseUIScreen*> configScreens;

    BaseUIScreen::BaseUIScreen(ProgramStage programStage, std::map<ProgramStage, BaseUIScreen*>* map) {
        if(map == nullptr) return;
        (*map)[programStage] = this;
    }
}