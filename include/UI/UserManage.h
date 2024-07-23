#ifndef BAKERMAKER_USERMANAGE_H
#define BAKERMAKER_USERMANAGE_H

#include "UI/BaseUIScreen.h"
#include "utils.h"

namespace bakermaker {
    // UI to manage users
    class UserManage : public BaseUIScreen {
        char newName[USERLENGTH] = {};

    public:
        UserManage();
        void render() override;
    };
}

#endif //BAKERMAKER_USERMANAGE_H
