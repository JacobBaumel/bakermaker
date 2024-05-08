//
// Created by skipp on 4/30/2024.
//

#ifndef BAKERMAKER_SERVERINSTALL_H
#define BAKERMAKER_SERVERINSTALL_H

#include "UI/BaseUIScreen.h"
#include <mutex>
#include <thread>
#include <atomic>

namespace bakermaker {
    class ServerInstall : public BaseUIScreen {
    private:
        static constexpr char SEPARATOR_STR[63] = "\n\n---------------------------------------------------------\n\n\0";

        char e[1] = {' '};
        ST::string instructions;

        ImGuiTextBuffer commandProgress;
        std::mutex bufferMutex;
        std::thread* exec;
        std::atomic_int execProgress;
        std::atomic_bool execDone;
        bool hasStartedExec;
        bool showCommandOutputs;

        std::mutex curcmd;
        ST::string curcmdstr;

    public:
        ServerInstall();
        void render() override;
    };
}

#endif //BAKERMAKER_SERVERINSTALL_H
