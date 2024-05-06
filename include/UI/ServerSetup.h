//
// Created by skipp on 4/28/2024.
//

#ifndef TEST_MD_SERVERSETUP_H
#define TEST_MD_SERVERSETUP_H

#include "UI/BaseUIScreen.h"

namespace bakermaker {
    class ServerSetup : public BaseUIScreen {
    private:
        static constexpr int BUFFER_LENGTH = 150;
        char c1[150]{'\0'};
        char c2[150]{'\0'};
        char c3[150]{'\0'};

    public:
        ServerSetup();
        void render() override;
    };
}

#endif //TEST_MD_SERVERSETUP_H
