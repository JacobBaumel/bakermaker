#include "UI/RepoManage.h"
#include "imgui.h"
#include <fstream>
#include "string_theory/iostream"
#include "string_theory/stdio"
#include "utils.h"

namespace bakermaker {
    ST::string getLine(std::ifstream& stream) {
        std::string line;
        std::getline(stream, line, '\n');
        return ST::string{line};
    }

    RepoManage::RepoManage()
            :
            BaseUIScreen(bakermaker::ProgramStage::REPO_MANAGE, &bakermaker::configScreens),
            repos(), reponames(), selectedRepo(""), selectedName(1) {
        reset();
    }

    void RepoManage::render() {
        using namespace ST::literals;

        if(!config["synced"].get<bool>()) ImGui::BeginDisabled();
        ImGui::PushFont(fontlist[1]);
        ImGui::Text("Manage Repositories");
        ImGui::PopFont();
        ImGui::Separator();

        if(ImGui::Button("save")) save();

        ImGui::Text("Enter New Repository Name: ");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(600);
        ImGui::InputText("##newrepo_name", newrepo, USERLENGTH);
        ImGui::SameLine();

        if(ImGui::Button("Create")) {
            if(reponames.contains(newrepo))
                bakermaker::startErrorModal("Repository already exists!");
            else {
                ST::string rname = newrepo;
                reponames.insert(rname);
                repos[rname] = std::set<RepoUser, RepoUserSort>();
                selectedRepo = rname;
                config["unsaved"] = true;
            }
        }
        ImGui::NewLine();

        if(!reponames.empty()) {
            ImGui::Text("Select repository to edit: ");
            ImGui::SameLine();
            ImGui::SetNextItemWidth(600);
            if(ImGui::BeginCombo("##repo_select_combo", selectedRepo.c_str())) {
                for(const ST::string& str: reponames) {
                    bool selected = str == selectedRepo;
                    if(ImGui::Selectable((str + "##reposelect").c_str(), &selected))
                        selectedRepo = str;
                    if(selected) ImGui::SetItemDefaultFocus();
                }

                ImGui::EndCombo();
            }

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
                    if(ImGui::Checkbox(("##isadmin"_st + it->name).c_str(), &it->isAdmin)) config["unsaved"] = true;

                    ImGui::TableNextColumn();
                    if(ImGui::Button(("Remove##"_st + it->name).c_str())) {
                        it = repos[selectedRepo].erase(it);
                        config["unsaved"] = true;
                    }

                    else it++;
                }

                ImGui::EndTable();
            }
        }

        ImGui::Text("Add user to repository: ");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(600);

        if(config["keys"].size() <= 1) {
            ImGui::Text("Please add users above first");
        }

        else {
            ImGui::SetNextItemWidth(600);
            if(ImGui::BeginCombo("##add_user_combo",
                                 config["keys"][selectedName].get<ST::string>().c_str())) {
                for(size_t i = 1; i < config["keys"].size(); i++) {
                    bool selected = selectedName == i;
                    if(ImGui::Selectable(
                            (config["keys"][i].get<ST::string>() + "##combo"_st).c_str(),
                            &selected))
                        selectedName = i;
                    if(selected) ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }

            ImGui::SameLine();
            if(ImGui::Button("Add##usercomboaddthing")) {
                ST::string user = config["keys"][selectedName].get<ST::string>();
                if(repos[selectedRepo].contains(RepoUser(user, false)))
                    bakermaker::startErrorModal("User already exists in repositor!");
                else {
                    repos[selectedRepo].insert(RepoUser(user, false));
                }
            }
        }


        if(!config["synced"].get<bool>()) ImGui::EndDisabled();
    }

    void RepoManage::reset() {
        using namespace ST::literals;
        std::ifstream conf("gitolite.conf");

        if(!conf.is_open()) {
            bakermaker::startErrorModal("Error reading config file. Try again.");
            config["synced"] = false;
            return;
        }

        ST::string currentRepo;

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
                for(const ST::string& str: line.after_first('=').trim().split(' ')) {
                    if(str.empty()) break;
                    repos[currentRepo].insert(RepoUser(str, true));
                }

                line = getLine(conf);

                for(const ST::string& str: line.after_first('=').trim().split(' ')) {
                    if(str.empty()) break;
                    repos[currentRepo].insert(RepoUser(str, false));
                }

                getLine(conf);
                getLine(conf);

                currentRepo.clear();
            }
        }
    }

    void RepoManage::save() {
        std::ofstream conf("gitolite.conf", std::ios::binary | std::ios::trunc);
        conf << "repo gitolite-admin\n\tRW+\t= admin\n\n";

        for(const auto& pair: repos) {
            ST::string admins, users;
            for(const auto& ru: pair.second) {
                if(ru.isAdmin) admins += ru.name + " "_st;
                else users += ru.name + " "_st;
            }

            conf << "repo " << pair.first << "\n\tRW+\t= " << admins << "\n\t- master\t= " << users
                 << "\n\tRW\t= " << users << "\n\n";
        }

        conf.close();
    }
}