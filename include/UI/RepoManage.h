//
// Created by skipp on 5/13/2024.
//

#ifndef BAKERMAKER_REPOMANAGE_H
#define BAKERMAKER_REPOMANAGE_H

#include "UI/BaseUIScreen.h"
#include <thread>
#include <atomic>
#include "string_theory/string"
#include <vector>
#include <mutex>

namespace bakermaker {
    class RepoManage : public BaseUIScreen {
    private:
        struct RepoString {
            ST::string header;
            ST::string rwp;
            ST::string master;
            ST::string rw;
        };

        std::thread* exec;
        std::atomic_bool execDone;
        std::atomic_int success;
        std::vector<RepoString> repos;
        std::vector<ST::string> ugroups;
        std::mutex vectormutex;

        void fetchRepoData(std::atomic_bool* execDone_, std::atomic_int* success_);
        void writeRepoData(std::atomic_bool* execDone_, std::atomic_int* success_);

    public:
        RepoManage();
        void render() override;
    };
}

#endif //BAKERMAKER_REPOMANAGE_H
