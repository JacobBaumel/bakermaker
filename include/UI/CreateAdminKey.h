#ifndef BAKERMAKER_CREATEADMINKEY_H
#define BAKERMAKER_CREATEADMINKEY_H

#include <thread>
#include <atomic>

#include "UI/BaseUIScreen.h"
#include "utils.h"

namespace bakermaker {
    // UI element to create the gitolite admin key
    class CreateAdminKey : public BaseUIScreen {
        // To store small blurb about the key to be rendered by the markdown renderer
        ST::string instructions;
        bool showSuccess;

    public:
        CreateAdminKey();
        void render() override;
    };
}

#endif //BAKERMAKER_CREATEADMINKEY_H
