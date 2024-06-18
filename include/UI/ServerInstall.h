//
// Created by skipp on 4/30/2024.
//

#ifndef BAKERMAKER_SERVERINSTALL_H
#define BAKERMAKER_SERVERINSTALL_H

#include "UI/BaseUIScreen.h"
#include <mutex>
#include <thread>
#include <atomic>
#include "libssh/libssh.h"

namespace bakermaker {
    class ServerInstall : public BaseUIScreen {
    private:
        static constexpr char SEPARATOR_STR[63] = "\n\n---------------------------------------------------------\n\n\0";
        int runSSHCommand(ssh_session sess, const char* command);
        void startNewCommand(const char* command);

        ST::string instructions;

        ImGuiTextBuffer commandProgress;
        std::mutex bufferMutex;
        std::thread* exec;
        std::atomic_int success;
        std::atomic_bool execDone;
        bool showCommandOutputs;

        std::mutex curcmd;
        ST::string curcmdstr;

        struct InstallParams {
            ST::string ip;
            int port;
            ST::string user;
            ST::string keyfile;
            ST::string adminkey;
            bool useiscsi;
            ST::string i1;
            ST::string i2;
            ST::string i3;
        };

        void install(const InstallParams& ip);

    public:
        ServerInstall();
        void render() override;
    };
}

#endif //BAKERMAKER_SERVERINSTALL_H
