#ifndef TEST_MD_SERVERSETUP_H
#define TEST_MD_SERVERSETUP_H

#include "UI/BaseUIScreen.h"

namespace bakermaker {
    // UI for entering ISCSI information, if applicable
    class ServerSetup : public BaseUIScreen {
        static constexpr int BUFFER_LENGTH = 150;
        char c1[BUFFER_LENGTH] = {};
        char c2[BUFFER_LENGTH] = {};
        char c3[BUFFER_LENGTH] = {};
        bool useiscsi;
        bool submitted;

    public:
        ServerSetup();
        void render() override;
    };
}

#endif //TEST_MD_SERVERSETUP_H
