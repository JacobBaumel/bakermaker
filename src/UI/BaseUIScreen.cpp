#include "UI/BaseUIScreen.h"

namespace bakermaker {
    ImFont** fontlist;
    std::map<ProgramStage, BaseUIScreen*> setupScreens;
    std::map<ProgramStage, BaseUIScreen*> configScreens;

    // If a child class provides one of the two maps above to be added to, add it to said map. Otherwise ignore.
    BaseUIScreen::BaseUIScreen(ProgramStage programStage, std::map<ProgramStage, BaseUIScreen*>* map) {
        if(map == nullptr) return;
        (*map)[programStage] = this;
    }
}