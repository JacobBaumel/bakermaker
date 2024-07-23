#ifndef BAKERMAKER_SYNCTOSERVER_H
#define BAKERMAKER_SYNCTOSERVER_H

#include <atomic>
#include <mutex>
#include <thread>

#include "string_theory/string"

#include "UI/BaseUIScreen.h"

namespace bakermaker {
    // UI to synchronize changes to and from the server
    class SyncToServer : public BaseUIScreen {
        std::thread* exec;
        std::atomic_bool execDone;
        std::atomic_int success;

        std::mutex statusmutex;
        ST::string status;

        // Internal helper function
        void setStatus(int rc);

        // Sync changes from the server to the program
        void syncFrom();

        // Upload changes from the program to the server. Takes in repository to delete, if relevant
        void syncTo(ST::string deleterepo);

    public:
        SyncToServer();
        void render() override;

        // Allows other UI elements to start a sync
        void startSync(const ST::string& deleterepo);
    };
}

#endif //BAKERMAKER_SYNCTOSERVER_H
