#ifndef DEBUGEXPORT_H
#define DEBUGEXPORT_H

#include "UI/BaseUIScreen.h"

namespace bakermaker {
    // UI element to show menu for creating debug.zip
    class DebugExport : public BaseUIScreen {
        bool showSuccess;

    public:
        DebugExport();
        void render() override;
    };
}

#endif //DEBUGEXPORT_H
