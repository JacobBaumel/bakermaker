#ifndef BAKERMAKER_USEDSTORAGE_H
#define BAKERMAKER_USEDSTORAGE_H

#include "UI/BaseUIScreen.h"
#include <thread>
#include <atomic>
#include <map>
#include <set>
#include "string_theory/string"
#include "libssh/libssh.h"

using namespace ST::literals;

namespace bakermaker {
    typedef int64_t storage_t;
    class UsedStorage : public BaseUIScreen {
    private:

        const ImU32 fg = 0xff00ff00;
        const ImU32 bg = 0xff303030;
        std::thread* exec;
        std::atomic_bool execDone;
        std::atomic_int success;

        std::atomic_llong totalAvail;
        std::atomic_llong totalUsed;
        std::map<ST::string, storage_t> repousage;
        std::set<ST::string> reponames;

        std::atomic_bool hasRefreshed;

        void refresh();
        static storage_t getSizeOfSomething(ssh_session sess, const ST::string& path, int option);

    public:
        UsedStorage();
        void render() override;
    };
}

#endif //BAKERMAKER_USEDSTORAGE_H
