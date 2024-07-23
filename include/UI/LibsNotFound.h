#ifndef BAKERMAKER_LIBSNOTFOUND_H
#define BAKERMAKER_LIBSNOTFOUND_H

#include "UI/BaseUIScreen.h"

namespace bakermaker {
    // UI for when openssl and ssh dll's are not available and need to be extracted from romfs
    class LibsNotFound : public BaseUIScreen {

    public:
        LibsNotFound();
        void render() override;
    };
}

#endif //BAKERMAKER_LIBSNOTFOUND_H
