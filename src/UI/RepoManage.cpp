#include <fstream>
#include <regex>

#include "imgui.h"
#include "string_theory/iostream"
#include "string_theory/string"

#include "UI/RepoManage.h"
#include "UI/SyncToServer.h"
#include "utils.h"

namespace bakermaker {
    using namespace ST::literals;

    // Function to get a line from an ifstream and write to ST::string
    ST::string getLine(std::ifstream& stream) {
        std::string line;
        std::getline(stream, line, '\n');
        return ST::string{line};
    }

    RepoManage::RepoManage() : BaseUIScreen(ProgramStage::REPO_MANAGE, &configScreens), selectedRepo(""),
                               selectedName(1), showConfirm(false) {
        reset();
    }

    void RepoManage::render() {
        // Header, and disable section if have not synced
        if(!config["synced"].get<bool>()) ImGui::BeginDisabled();
        ImGui::PushFont(fontlist[1]);
        ImGui::Text("Manage Repositories");
        ImGui::PopFont();
        ImGui::Separator();

        // Text field for new repository name
        ImGui::Text("Enter New Repository Name: ");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(600);
        const bool enter = ImGui::InputText("##newrepo_name", newrepo, USERLENGTH,
                                            ImGuiInputTextFlags_EnterReturnsTrue);
        ImGui::SameLine();

        if(enter || ImGui::Button("Create")) {
            // Check that newrepo is not empty, and that the repo does not already exist
            if(newrepo[0] == '\0') startErrorModal("Please enter a name!");

            else if(std::regex_search(
                newrepo, std::regex(R"([\(\)\{\}\[\]" "\@\#\$\%\^\&\*\!\.\,\/\\\<\>\;\:\'\"\`\~\-\+\=\|])"))) {
                startErrorModal(
                    R"(Name cannot contain one of the following characters: [](){}/\|,.<>?;:'"`~!@#$%^&*- or space)");
                }

            else if(reponames.contains(newrepo)) startErrorModal("Repository already exists!");
            else {
                const ST::string rname = newrepo;
                reponames.insert(rname);
                repos[rname] = std::set<RepoUser, RepoUserSort>();
                selectedRepo = rname;
                memset(newrepo, 0, USERLENGTH);
                if(enter) ImGui::SetKeyboardFocusHere(-1);
                config["unsaved"] = true;
            }
        }

        ImGui::NewLine();

        // Only show this UI if a repo exists
        if(!reponames.empty()) {
            // Dropdown selector for selecting which repository to edit
            ImGui::Text("Select repository to edit: ");
            ImGui::SameLine();
            ImGui::SetNextItemWidth(600);
            if(ImGui::BeginCombo("##repo_select_combo", selectedRepo.c_str())) {
                for(const ST::string& str : reponames) {
                    bool selected = str == selectedRepo;
                    if(ImGui::Selectable((str + "##reposelect").c_str(), &selected))
                        selectedRepo = str;
                    if(selected) ImGui::SetItemDefaultFocus();
                }

                ImGui::EndCombo();
            }

            // Table to manage user perms. Contains a row with name, admin checkbox, and delete button
            ImGui::PushFont(fontlist[3]);
            ImGui::Text("Manage Users");
            ImGui::PopFont();

            if(ImGui::BeginTable("table", 3,
                                 ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg, ImVec2(600, 0))) {
                ImGui::TableSetupColumn("Name");
                ImGui::TableSetupColumn("Is Admin");
                ImGui::TableSetupColumn("Remove");
                ImGui::TableHeadersRow();

                auto it = repos[selectedRepo].begin();
                while(it != repos[selectedRepo].end()) {
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::TextUnformatted(it->name.c_str());

                    ImGui::TableNextColumn();
                    if(ImGui::Checkbox(("##isadmin"_st + it->name).c_str(), &it->isAdmin))
                        config["unsaved"] = true;

                    ImGui::TableNextColumn();
                    if(ImGui::Button(("Remove##"_st + it->name).c_str())) {
                        it = repos[selectedRepo].erase(it);
                        config["unsaved"] = true;
                    }

                    else ++it;
                }

                ImGui::EndTable();
            }
        }

        // Dropdown selector to add additional users to repositories
        ImGui::Text("Add user to repository: ");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(600);

        // First check there are more than just the admin user created
        if(config["keys"].size() <= 1) {
            ImGui::Text("Please add users above first");
        }

        else {
            ImGui::SetNextItemWidth(600);
            if(ImGui::BeginCombo("##add_user_combo", config["keys"][selectedName].get<ST::string>().c_str())) {
                for(size_t i = 1; i < config["keys"].size(); i++) {
                    bool selected = selectedName == i;
                    if(ImGui::Selectable((config["keys"][i].get<ST::string>() + "##combo"_st).c_str(), &selected))
                        selectedName = i;
                    if(selected) ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }

            ImGui::SameLine();
            if(ImGui::Button("Add##usercomboaddthing")) {
                ST::string user = config["keys"][selectedName].get<ST::string>();
                if(repos[selectedRepo].contains(RepoUser(user, false)))
                    startErrorModal("User already exists in repositor!");
                else repos[selectedRepo].insert(RepoUser(user, false));
            }
        }

        // Delete button for repository
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1, 0, 0, 1));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.9, 0, 0, 1));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1, 0, 0, 1));
        if(ImGui::Button("Delete Repository##repodelete")) showConfirm = true;
        ImGui::PopStyleColor(3);

        bool confirmed = false;

        // Confirmation for deleting
        if(showConfirm) {
            ImVec2 size{525, 175};
            ImGui::SetNextWindowSize(size);
            ImGui::SetNextWindowFocus();
            ImVec2 screenSize = ImGui::GetIO().DisplaySize;
            screenSize.x = (screenSize.x - size.x) / 2;
            screenSize.y = (screenSize.y - size.y) / 2;
            ImGui::SetNextWindowPos(screenSize);
            if(ImGui::Begin("Confirm Delete", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoDecoration)) {
                ImGui::Text("Confirm deleting repository \"%s\"", selectedRepo.c_str());
                ImGui::PushFont(fontlist[3]);
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 0, 0, 1));
                ImGui::TextWrapped(
                    "WARNING: This action is irreversible. All data belonging to %s will be deleted from the git server.",
                    selectedRepo.c_str());
                ImGui::PopFont();
                ImGui::PopStyleColor();
                ImGui::TextWrapped(
                    "Following a confirmation, the software will save changes to the server immediately.");

                ImGui::NewLine();
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1, 0, 0, 1));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.9, 0, 0, 1));
                ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1, 0, 0, 1));
                if(ImGui::Button("Confirm")) {
                    showConfirm = false;
                    repos.erase(selectedRepo);
                    reponames.erase(selectedRepo);
                    ((SyncToServer*)configScreens[ProgramStage::SYNC_TO_SERVER])->startSync(selectedRepo);
                    selectedRepo.clear();
                    confirmed = true;
                }
                ImGui::PopStyleColor(3);

                ImGui::SameLine();
                if(ImGui::Button("Cancel")) showConfirm = false;
                ImGui::End();
            }
        }

        if(!confirmed && !config["synced"].get<bool>()) ImGui::EndDisabled();
    }

    int RepoManage::reset() {
        // Check if file exists
        if(!std::filesystem::exists("gitolite.conf") ||
            std::filesystem::is_directory("gitolite.conf"))
            return -1;

        std::ifstream conf("gitolite.conf");

        if(!conf.is_open()) {
            startErrorModal("Error reading config file. Try again.");
            config["synced"] = false;
            return -2;
        }

        ST::string currentRepo;

        /* The expected format of the config file is as follows:
         *
         * First 2 lines:
         * repo gitolite-admin
         *      RW+ admin
         *
         * Following that, more repositories can be added with the following format, with 1 blank line between them:
         * repo <name>
         * \tRW+\t= admin <list of admin users for this repo>
         * \t-\tmaster\t= <list of normal users for this repo>
         * \tRW\t= <list of normal users for this repo>
         *
         */
        while(!conf.eof()) {
            ST::string line = getLine(conf);
            if(line.empty()) continue;

            if(currentRepo.empty()) {
                if(!line.starts_with("repo ")) continue;
                currentRepo = line.substr(5);
                if(currentRepo == "gitolite-admin"_st) {
                    getLine(conf);
                    currentRepo.clear();
                    continue;
                }

                if(selectedRepo.empty()) selectedRepo = currentRepo;
                reponames.insert(currentRepo);
                repos[currentRepo] = std::set<RepoUser, RepoUserSort>();
            }

            else {
                for(const ST::string& str : line.after_first('=').trim().split(' ')) {
                    if(str.empty()) break;
                    if(str == "admin") continue;
                    repos[currentRepo].insert(RepoUser(str, true));
                }

                line = getLine(conf);

                for(const ST::string& str : line.after_first('=').trim().split(' ')) {
                    if(str.empty()) break;
                    repos[currentRepo].insert(RepoUser(str, false));
                }

                getLine(conf);
                getLine(conf);

                currentRepo.clear();
            }
        }

        return 0;
    }

    int RepoManage::save() {
        std::ofstream conf("gitolite.conf", std::ios::binary | std::ios::trunc);
        conf << "repo gitolite-admin\n\tRW+\t= admin\n\n";

        for(const auto& pair : repos) {
            ST::string admins = "admin "_st, users;
            for(const auto& ru : pair.second) {
                if(ru.isAdmin) admins += ru.name + " "_st;
                else users += ru.name + " "_st;
            }

            conf << "repo " << pair.first << "\n\tRW+\t= " << admins << "\n\t- master\t= " << users
                << "\n\tRW\t= " << users << "\n\n";
        }

        conf.close();

        return 0;
    }

    void RepoManage::deleteUser(const ST::string& user) {
        // Search all repositories and remove the specified user from it if found
        for(auto& repo : repos) {
            std::erase_if(repo.second, [&user](const RepoUser& r) { return r.name == user; });
        }

        if(config["keys"][selectedName].get<ST::string>() == user) selectedName = 1;
    }

    const std::set<ST::string>& RepoManage::getRepoNames() {
        return reponames;
    }
}
