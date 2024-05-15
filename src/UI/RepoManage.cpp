#include "UI/RepoManage.h"
#include "imgui.h"
#include <chrono>
#include <fstream>
#include "string_theory/iostream"
#include "string_theory/stdio"
#include "utils.h"

namespace bakermaker {
    ST::string getLine(std::ifstream& stream) {
        std::string line;
        std::getline(stream, line);
        return ST::string(line);
    }

    RepoManage::RepoManage() : BaseUIScreen(bakermaker::ProgramStage::REPO_MANAGE), exec(nullptr),
        execDone(false), success(0), ugroups(), repos(), vectormutex(), selectedRepo(0), reponames() {}

    void RepoManage::render() {
        ImGui::PushFont(fontlist[1]);
        ImGui::Text("Manage Repositories");
        ImGui::PopFont();
        ImGui::Separator();

        if(exec && !execDone) ImGui::BeginDisabled();
        if(ImGui::Button("Refresh Repository Data")) {
            execDone = false;
            exec = new std::thread(&RepoManage::fetchRepoData, this, &execDone, &success);
            ImGui::BeginDisabled();
        }

        if(exec && !execDone) {
            ImGui::EndDisabled();
            ImGui::SameLine();
            bakermaker::spinner();
        }

        if(execDone) {
            if(exec) {
                exec->join();
                delete exec;
                exec = nullptr;

                if(success != 0) bakermaker::startErrorModal("adwsefrgth");
            }
        }

        if(vectormutex.try_lock()) {
            ImGui::Text("Enter New Repository Name: ");
            ImGui::SameLine();
            ImGui::SetNextItemWidth(600);
            ImGui::InputText("##newrepo_name", newrepo, NEW_REPO_NAME_LENGTH);
            ImGui::SameLine();
            if(ImGui::Button("Create")) {
                if(std::find(reponames.begin(), reponames.end(), ST::string(newrepo)) != reponames.end())
                    bakermaker::startErrorModal("Repository already exists!");

                else {
                    ST::string rname = newrepo;
                    reponames.push_back(rname); // NOLINT(*-use-emplace)
                    RepoString newrepostring{REPO_LINE + rname, RWP_LINE + rname + "admin",
                                             MASTER_LINE + rname, RW_LINE + rname};
                    repos[rname] = newrepostring;

                    ugroups[rname] = {};
                    ugroups[rname + "admin"] = {};
                }
            }
            ImGui::NewLine();

            if(!reponames.empty()) {
                ImGui::Text("Select repository to edit: ");
                ImGui::SameLine();
                ImGui::SetNextItemWidth(600);
                if(ImGui::BeginCombo("##repo_select_combo", reponames[selectedRepo].c_str())) {
                    for(size_t i = 0; i < reponames.size(); i++) {
                        bool selected = reponames[selectedRepo] == reponames[i];
                        if(ImGui::Selectable(reponames[i].c_str(), &selected)) selectedRepo = i;
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

                    std::vector<RepoUser>& users = ugroups[reponames[selectedRepo]];
                    auto it = users.begin();
                    while(it != users.end()) {
                        ImGui::TableNextRow();

                        ImGui::TableNextColumn();
                        ImGui::TextUnformatted(it->name.c_str());

                        ImGui::TableNextColumn();
                        ImGui::Checkbox((ST::string("##isadmin") + it->name).c_str(), &it->isAdmin);

                        ImGui::TableNextColumn();
                        if(ImGui::Button((ST::string("Remove##") + it->name).c_str()))
                            it = users.erase(it);

                        else it++;
                    }

                    ImGui::EndTable();
                }
            }

            ImGui::Text("Add user to repository: ");
            ImGui::SameLine();
            ImGui::SetNextItemWidth(600);

            if(ImGui::BeginCombo("##add_user_combo", "e")) {
                ImGui::EndCombo();
            }

            vectormutex.unlock();
        }


    }

    void RepoManage::fetchRepoData(std::atomic_bool* execDone_, std::atomic_int* success_) {
        vectormutex.lock();
        using namespace std::chrono_literals;
        std::this_thread::sleep_for(2000ms);

        std::ifstream repoconfig("gitolite.conf");

        if(!repoconfig.is_open()) {
            *success_ = -1;
            *execDone_ = true;
        }

        ugroups.clear();
        repos.clear();

        while(!repoconfig.eof()) {
            ST::string line = getLine(repoconfig);

            if(line.starts_with("@")) {
                ST::string groupName = line.before_first(' ').substr(2);
                std::vector<ST::string> users = line.after_first(' ').after_first(' ').split(' ');
                if(!ugroups.contains(groupName)) ugroups[groupName] = {};
                for(const auto& u : users)
                    ugroups[groupName].push_back({u, line[1] == 'a'});
            }

            else if(line.starts_with("repo ")) {
                if(line.contains("gitolite-admin")) {
                    getLine(repoconfig);
                    continue;
                }

                RepoString repo{};
                repo.header = line;

                repo.rwp = getLine(repoconfig);
                repo.master = getLine(repoconfig);
                repo.rw = getLine(repoconfig);

                ST::string name = repo.header.substr(5, repo.header.size() - 1);
                repos[name] = repo;
                reponames.push_back(name);
            }
        }

        *execDone_ = true;
        *success_ = 0;

        vectormutex.unlock();
    }

    void RepoManage::writeRepoData(std::atomic_bool* execDone_, std::atomic_int* success_) {

    }
}