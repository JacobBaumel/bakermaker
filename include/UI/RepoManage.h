#ifndef BAKERMAKER_REPOMANAGE_H
#define BAKERMAKER_REPOMANAGE_H

#include <map>
#include <set>

#include "utils.h"
#include "string_theory/string"

#include "UI/BaseUIScreen.h"
#include "UI/SyncToServer.h"

namespace bakermaker {
    // Structure to contain the name and admin status in one place
    struct RepoUser {
        ST::string name;
        mutable bool isAdmin;

        bool operator==(const RepoUser& other) const {
            return name == other.name;
        }
    };

    // std::set sorting for RepoUser
    struct RepoUserSort {
        bool operator()(const RepoUser& lhs, const RepoUser& rhs) const {
            return lhs.name < rhs.name;
        }
    };

    // UI for managing repositories on gitolite server
    class RepoManage : public BaseUIScreen {
        std::map<ST::string, std::set<RepoUser, RepoUserSort>> repos;
        std::set<ST::string> reponames;

        char newrepo[USERLENGTH] = {};

        ST::string selectedRepo;
        size_t selectedName;
        bool showConfirm;

    public:
        RepoManage();
        void render() override;

        // Function for sync to trigger reloading the gitolite.conf file into memory
        int reset();

        // Function for sync to trigger writing config changes to gitolite.conf from memory
        int save();

        // Function for user manage to delete a user from all repositories
        void deleteUser(const ST::string& user);

        // Function for used storage to get a list of repositories
        const std::set<ST::string>& getRepoNames();
    };
}

#endif //BAKERMAKER_REPOMANAGE_H