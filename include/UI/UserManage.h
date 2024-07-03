#ifndef BAKERMAKER_USERMANAGE_H
#define BAKERMAKER_USERMANAGE_H

#include "UI/BaseUIScreen.h"
#include "utils.h"
#include <thread>
#include <atomic>

namespace bakermaker {
    class UserManage : public BaseUIScreen {
    private:
        char newName[USERLENGTH] = {'\0'};

    public:
        UserManage();
        void render() override;
    };
}

#endif //BAKERMAKER_USERMANAGE_H
