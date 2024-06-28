#ifndef BAKERMAKER_SYNCTOSERVER_H
#define BAKERMAKER_SYNCTOSERVER_H

#include "UI/BaseUIScreen.h"

#include <thread>
#include <atomic>

namespace bakermaker {
    class SyncToServer : public BaseUIScreen {
    private:
        static const int SFTP_BUF_SIZE = 128;

        std::thread* exec;
        std::atomic_bool execDone;
        std::atomic_int success;

        void syncFrom();
        void syncTo();

    public:
        SyncToServer();
        void render() override;
    };
}

#endif //BAKERMAKER_SYNCTOSERVER_H
