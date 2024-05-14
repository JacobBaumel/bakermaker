#include "UI/CreateSSHKeys.h"
#include "UI/BaseUIScreen.h"
#include "romfs/romfs.hpp"
#include "utils.h"
#include <filesystem>
#include "ssh_helper.h"
#include <iostream>
#include "improgress.h"
#include <chrono>

namespace bakermaker {
    CreateSSHKeys::CreateSSHKeys() : BaseUIScreen(bakermaker::ProgramStage::SSH_KEYGEN),
        exec(nullptr), success(0), newName(new char[64]{'\0'}), execDone(false) {
        romfs::Resource text = romfs::get("SSHKeyGen.md");
        instructions = ST::string((char*) text.data(), text.size());

        if(!std::filesystem::exists("keys") && !std::filesystem::is_directory("keys"))
            std::filesystem::create_directories("keys");
    }

    void CreateSSHKeys::render() {
        ImGui::PushFont(fontlist[1]);
        ImGui::Text("Create SSH Keys");
        ImGui::PopFont();
        ImGui::Separator();

        bakermaker::documentation->render(instructions);
        ImGui::NewLine();

        ImGui::Text("Name: ");
        if(exec && !execDone) ImGui::BeginDisabled();
        ImGui::SetNextItemWidth(600);
        ImGui::InputText("##newuserenter", newName, 64);
        ImGui::SameLine();

        if(ImGui::Button("Add User")) {
            execDone = false;
            if(std::find(users.begin(), users.end(), std::string(newName)) != users.end()) {
                bakermaker::startErrorModal((ST::string("User ") + newName + " has already been added.").c_str());
            }
            else {
                exec = new std::thread(&CreateSSHKeys::createUser, newName, &execDone, &success, &users);
            }
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

                switch(success) {
                    case 1:
                        bakermaker::startErrorModal("Error when adding user");
                        break;

                    case -1:
                        bakermaker::startErrorModal("Error when deleting user");
                }
            }

            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0, 1, 0, 1));
            ImGui::Text("Success!");
            ImGui::PopStyleColor();
        }

        ImGui::NewLine();

        ImGui::PushFont(fontlist[2]);
        ImGui::Text("Added Users:");
        ImGui::PopFont();

        if(ImGui::BeginTable("table", 2,
                             ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg, ImVec2(600, 0))) {
            for(const auto& name : users) {
                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::Text("%s", name);
                ImGui::TableNextColumn();
                if(ImGui::Button((ST::string("Delete##") + name).c_str())) {
                    execDone = false;
                    exec = new std::thread(&CreateSSHKeys::deleteUser, name, &execDone, &success, &users);
                }
            }
            ImGui::EndTable();
        }


//
//        if(hasConfirmed) ImGui::BeginDisabled();
//        ImGui::Text("Admin User: ");
//        ImGui::SameLine();
//        ImGui::SetNextItemWidth(600);
//        ImGui::InputText("##admin_enter", adminname, USERLENGTH);
//
//        ImGui::NewLine();
//        ImGui::Text("Enter Regular Users:");
//        ImGui::NewLine();
//        ImGui::Text("Name:                                                    "
//                    "     Permission (true = advanced, false = normal");
//
//        int i = 0;
//        for(const auto& entry : users) {
//            ImGui::SetNextItemWidth(600);
//            ImGui::InputText((ST::string("##usernameinput") + i).c_str(), entry.first, USERLENGTH);
//            ImGui::SameLine();
//            ImGui::Checkbox(((ST::string("##usercheckbox") + i).c_str()), entry.second);
//            i++;
//        }
//
//        if(ImGui::Button("Add more users")) {
//            char* newbuf = new char[USERLENGTH];
//            memset((void*) newbuf, 0, USERLENGTH);
//            bool* newbool = new bool(false);
//
//            users[newbuf] = newbool;
//        }
//
//        ImGui::NewLine();
//        if(ImGui::Button("Confirm Users")) {
//            hasConfirmed = true;
//
//            Json(config["keys"]) << Json::object();
//            config["keys"][0]["name"] = adminname;
//            config["keys"][0]["perm"] = 0;
//
//            bool yes = true;
//            users[adminname] = &yes;
//
//            for(const auto& entry : users) {
//                int length = Json(config["keys"]).size();
//                Json(config["keys"]) << Json::object();
//                config["keys"][length]["name"] = entry.first;
//                config["keys"][length]["perm"] = *entry.second ? 1 : 2;
//            }
//
//            exec = new std::thread([this](){
//                execDone = false;
//                execProgress = 0;
//                execTotal = users.size();
//
//                for(const auto& entry : users) {
//                    int rc = genSSHKeyToFile(("keys/" + ST::string(entry.first)).c_str());
//                    std::cout << entry.first << ": " << rc << std::endl;
//                    execProgress++;
//                }
//
//                execDone = true;
//            });
//        }
//        else if(hasConfirmed) ImGui::EndDisabled();
//
//        if(hasConfirmed) {
//            ImGui::Text("Generating keys: %i / %llu", execProgress.load(), execTotal.load());
//
//            if(!execDone) {
//                ImGui::SameLine();
//                ImGui::Spinner("##libs_extract", 10, 1,
//                               ImGui::GetColorU32(ImVec4(0.1, 0.1, 0.75, 1)));
//            }
//
//            else {
//                if(exec != nullptr) {
//                    exec->join();
//                    delete exec;
//                    exec = nullptr;
//                }
//
//                ImGui::Text("Key Generation Complete");
//            }
//        }
    }

    void CreateSSHKeys::createUser(const char* name, std::atomic_bool* execDone,
                                   std::atomic_int* success, std::vector<char*>* users) {
        using namespace std::chrono_literals;
        std::this_thread::sleep_for(2000ms);

        size_t len = strlen(name);
        char* namec = new char[len];
        namec[len] = '\0';
        memcpy(namec, name, len);
        users->push_back(namec);

        *success = 0;
        *execDone = true;
    }

    void CreateSSHKeys::deleteUser(const char* name, std::atomic_bool* execDone,
                                   std::atomic_int* success, std::vector<char*>* users) {
        using namespace std::chrono_literals;
        std::this_thread::sleep_for(2000ms);

        users->erase(std::find(users->begin(), users->end(), std::string(name)));

        *success = 0;
        *execDone = true;
    }
}