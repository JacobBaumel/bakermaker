#ifndef BAKERMAKER_USEDSTORAGE_H
#define BAKERMAKER_USEDSTORAGE_H

#include <atomic>
#include <map>
#include <set>
#include <thread>

#include "libssh/libssh.h"
#include "string_theory/string"

#include "UI/BaseUIScreen.h"

namespace bakermaker {
    typedef int64_t storage_t;

    // UI to show how much storage has been used on the server
    class UsedStorage : public BaseUIScreen {
        std::thread* exec;
        std::atomic_bool execDone;
        std::atomic_int success;

        std::atomic_llong totalAvail;
        std::atomic_llong totalUsed;
        std::map<ST::string, storage_t> repousage;
        std::set<ST::string> reponames;

        std::atomic_bool hasRefreshed;

        // Function which refreshes all storage values. To be run in a thread.
        void refresh();

        // Helper function for actually retrieving the size of a directory
        static storage_t getSizeOfSomething(ssh_session sess, const ST::string& path, int option);

    public:
        UsedStorage();
        void render() override;
    };
}

#endif //BAKERMAKER_USEDSTORAGE_H
