#ifndef BAKERMAKER_SYNCTOSERVER_H
#define BAKERMAKER_SYNCTOSERVER_H

#include "UI/BaseUIScreen.h"

#include <thread>
#include <atomic>
#include <mutex>
#include "string_theory/string"

namespace bakermaker {
    class SyncToServer : public BaseUIScreen {
    private:
        static const int SFTP_BUF_SIZE = 128;

        std::thread* exec;
        std::atomic_bool execDone;
        std::atomic_int success;

        std::mutex statusmutex;
        ST::string status;

        void setStatus(int rc);
        void syncFrom();
        void syncTo();

    public:
        SyncToServer();
        void render() override;
    };
}

#endif //BAKERMAKER_SYNCTOSERVER_H
