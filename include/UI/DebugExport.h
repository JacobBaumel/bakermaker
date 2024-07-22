#ifndef DEBUGEXPORT_H
#define DEBUGEXPORT_H

#include "UI/BaseUIScreen.h"

namespace bakermaker {
    class DebugExport : public BaseUIScreen {
    public:
        DebugExport();
        void render() override;
    };
}

#endif //DEBUGEXPORT_H
