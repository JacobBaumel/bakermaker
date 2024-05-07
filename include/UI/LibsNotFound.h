#ifndef BAKERMAKER_LIBSNOTFOUND_H
#define BAKERMAKER_LIBSNOTFOUND_H

#include "UI/BaseUIScreen.h"
#include <thread>
#include <atomic>
#include "romfs/romfs.hpp"

namespace bakermaker {
    class LibsNotFound : public BaseUIScreen {
    private:
        static constexpr ImVec2 MODAL_SIZE{525, 130};
        static constexpr ImGuiWindowFlags modalFlags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_Modal;

        std::thread* extrThread;
        std::atomic_bool extrDone;

    public:
        LibsNotFound();
        void render() override;

    };
}

#endif //BAKERMAKER_LIBSNOTFOUND_H
