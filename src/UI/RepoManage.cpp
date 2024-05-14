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
        execDone(false), success(0), ugroups(), repos(), vectormutex() {}

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

            if(line.starts_with("@u")) {
                ugroups.push_back(line);
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

                repos.push_back(repo);
            }
        }

        *execDone_ = true;
        *success_ = 0;

        vectormutex.unlock();
    }

    void RepoManage::writeRepoData(std::atomic_bool* execDone_, std::atomic_int* success_) {

    }
}