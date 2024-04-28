#include "BaseUIScreen.h"

namespace bakermaker {
    BaseUIScreen::BaseUIScreen(ProgramStage programStage) {
        screens[programStage] = this;
    }
}