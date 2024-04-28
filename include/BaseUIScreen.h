//
// Created by skipp on 4/27/2024.
//

#ifndef BAKERMAKER_BASEUISCREEN_H
#define BAKERMAKER_BASEUISCREEN_H

#include <map>

namespace bakermaker {
    enum class ProgramStage {
        START
    };

    class BaseUIScreen {
    public:

        BaseUIScreen(ProgramStage programStage);
        virtual void render(ProgramStage& stage, int latestId) = 0;
    };

    static std::map<ProgramStage, BaseUIScreen*> screens;
}

#endif //BAKERMAKER_BASEUISCREEN_H
