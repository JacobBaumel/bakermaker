#include "UI/RepoManage.h"
#include "imgui.h"
#include <chrono>
#include <fstream>
#include <iostream>
#include "string_theory/iostream"
#include "string_theory/stdio"
#include "utils.h"

namespace bakermaker {
    ST::string getLine(std::ifstream& stream) {
        std::string line;
        std::getline(stream, line);
        return ST::string(line);
    }

    RepoManage::RepoManage()
            :
            BaseUIScreen(bakermaker::ProgramStage::REPO_MANAGE, &bakermaker::configScreens),
            repos(), reponames(), selectedRepo("") {
        reset();
    }

    void RepoManage::render() {
        using namespace ST::literals;

        if(!config["synced"].get<bool>()) ImGui::BeginDisabled();
        ImGui::PushFont(fontlist[1]);
        ImGui::Text("Manage Repositories");
        ImGui::PopFont();
        ImGui::Separator();

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
                    ImGui::Checkbox(("##isadmin"_st + it->name).c_str(), &it->isAdmin);

                    ImGui::TableNextColumn();
                    if(ImGui::Button(("Remove##"_st + it->name).c_str()))
                        it = repos[selectedRepo].erase(it);

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

//    void RepoManage::fetchRepoData(std::atomic_bool* execDone_, std::atomic_int* success_) {
//        vectormutex.lock();
//        using namespace std::chrono_literals;
//        std::this_thread::sleep_for(2000ms);
//
//        std::ifstream repoconfig("gitolite.conf");
//
//        if(!repoconfig.is_open()) {
//            *success_ = -1;
//            *execDone_ = true;
//        }
//
//        ugroups.clear();
//        repos.clear();
//
//        while(!repoconfig.eof()) {
//            ST::string line = getLine(repoconfig);
//
//            if(line.starts_with("@")) {
//                ST::string groupName = line.before_first(' ').substr(2);
//                std::vector<ST::string> users = line.after_first(' ').after_first(' ').split(' ');
//                if(!ugroups.contains(groupName)) ugroups[groupName] = {};
//                for(const auto& u: users)
//                    ugroups[groupName].push_back({u, line[1] == 'a'});
//            }
//
//            else if(line.starts_with("repo ")) {
//                if(line.contains("gitolite-admin")) {
//                    getLine(repoconfig);
//                    continue;
//                }
//
//                RepoString repo{};
//                repo.header = line;
//
//                repo.rwp = getLine(repoconfig);
//                repo.master = getLine(repoconfig);
//                repo.rw = getLine(repoconfig);
//
//                ST::string name = repo.header.substr(5, repo.header.size() - 1);
//                repos[name] = repo;
//                reponames.push_back(name);
//            }
//        }
//
//        *execDone_ = true;
//        *success_ = 0;
//
//        vectormutex.unlock();
//    }
//
//    void RepoManage::writeRepoData(std::atomic_bool* execDone_, std::atomic_int* success_) {
//
//    }

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

            if(currentRepo.empty()) {
                if(!line.starts_with("repo ")) continue;
                currentRepo = line.substr(5);
                if(currentRepo == "gitolite-admin"_st) {
                    getLine(conf);
                    currentRepo.clear();
                    continue;
                }

                reponames.insert(currentRepo);
                repos[currentRepo] = std::set<RepoUser, RepoUserSort>();
            }

            else {
                for(const ST::string& str : line.after_first('=').trim().split(' ')) {
                    repos[currentRepo].insert(RepoUser(str, true));
                }

                line = getLine(conf);

                for(const ST::string& str : line.after_first('=').trim().split(' ')) {
                    repos[currentRepo].insert(RepoUser(str, false));
                }

                getLine(conf);
                getLine(conf);

                currentRepo.clear();
            }
        }
    }

    void RepoManage::save() {

    }
}