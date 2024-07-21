#ifndef BAKERMAKER_REPOMANAGE_H
#define BAKERMAKER_REPOMANAGE_H

#include "UI/BaseUIScreen.h"
#include "UI/SyncToServer.h"
#include "utils.h"
#include "string_theory/string"
#include <set>
#include <map>

namespace bakermaker {
    using namespace ST::literals;

    class RepoManage : public BaseUIScreen {
    private:
        struct RepoUser {
            ST::string name;
            mutable bool isAdmin;

            bool operator== (const RepoUser& other) const {
                return name == other.name;
            }
        };

        struct RepoUserSort {
            bool operator() (const RepoUser& lhs, const RepoUser& rhs) const {
                return lhs.name < rhs.name;
            }
        };

        std::map<ST::string, std::set<RepoUser, RepoUserSort>> repos;
        std::set<ST::string> reponames;

        char newrepo[USERLENGTH] = {'\0'};

        ST::string selectedRepo;
        size_t selectedName;
        bool showConfirm;


    public:
        RepoManage();
        void render() override;
        void reset();
        void save();
        void deleteUser(const ST::string& user);
        const std::set<ST::string>& getRepoNames();
    };
}

#endif //BAKERMAKER_REPOMANAGE_H
