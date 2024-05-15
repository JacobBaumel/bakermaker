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
#include <map>
#include <mutex>

namespace bakermaker {
    using namespace ST::literals;

    class RepoManage : public BaseUIScreen {
    private:
        static constexpr int NEW_REPO_NAME_LENGTH = 64;

        const ST::string REPO_LINE   = "repo "_st;
        const ST::string RWP_LINE    = "    RW+      = @u"_st;
        const ST::string MASTER_LINE = "    - master = @u"_st;
        const ST::string RW_LINE     = "    RW       = @u"_st;

        struct RepoString {
            ST::string header;
            ST::string rwp;
            ST::string master;
            ST::string rw;
        };

        struct RepoUser {
            ST::string name;
            bool isAdmin;
        };

        std::thread* exec;
        std::atomic_bool execDone;
        std::atomic_int success;
        std::map<ST::string, RepoString> repos;
        std::vector<ST::string> reponames;
        std::map<ST::string, std::vector<RepoUser>> ugroups;
        std::mutex vectormutex;

        char newrepo[NEW_REPO_NAME_LENGTH]{'\0'};

        size_t selectedRepo;

        void fetchRepoData(std::atomic_bool* execDone_, std::atomic_int* success_);
        void writeRepoData(std::atomic_bool* execDone_, std::atomic_int* success_);

    public:
        RepoManage();
        void render() override;
    };
}

#endif //BAKERMAKER_REPOMANAGE_H
