#ifndef TEST_MD_SERVERCONNECT_H
#define TEST_MD_SERVERCONNECT_H

#include <atomic>
#include <thread>

#include "imfilebrowser.h"

#include "UI/BaseUIScreen.h"

namespace bakermaker {
    // UI for entering server connection information
    class ServerConnect : public BaseUIScreen {
        char ip[16]{};
        char user[33]{};
        int port = 22;
        ImGui::FileBrowser* browser;
        std::thread* exec;
        std::atomic_bool execDone;
        std::atomic_int success;

    public:
        ServerConnect();
        void render() override;
    };
}

#endif //TEST_MD_SERVERCONNECT_H
