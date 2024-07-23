#ifndef BAKERMAKER_SERVERINSTALL_H
#define BAKERMAKER_SERVERINSTALL_H

#include <atomic>
#include <mutex>
#include <thread>

#include "libssh/libssh.h"
#include "string_theory/string"

#include "UI/BaseUIScreen.h"

namespace bakermaker {
    // Struct to easily pass all parameters to the threaded installation method
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

    // UI for server installation
    class ServerInstall : public BaseUIScreen {
        // Wrapper for running a command, and writing the output to commandProgress
        int runSSHCommand(ssh_session sess, const char* command);

        ST::string instructions;

        ImGuiTextBuffer commandProgress;
        std::mutex bufferMutex;
        std::thread* exec;
        std::atomic_int success;
        std::atomic_bool execDone;
        bool showCommandOutputs;

        // Function that actually performs the installation. To be run in a thread
        void install(const InstallParams& ip);

    public:
        ServerInstall();
        void render() override;
    };
}

#endif //BAKERMAKER_SERVERINSTALL_H