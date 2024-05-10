#include "UI/CreateSSHKeys.h"
#include "UI/BaseUIScreen.h"
#include "romfs/romfs.hpp"
#include "setup.h"
#include <filesystem>
#include "ssh_helper.h"
#include <iostream>
#include "improgress.h"

namespace bakermaker {
    CreateSSHKeys::CreateSSHKeys() : BaseUIScreen(bakermaker::ProgramStage::SSH_KEYGEN),
        hasConfirmed(false), execDone(false), execProgress(0), execTotal(0) {
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

        if(hasConfirmed) ImGui::BeginDisabled();
        ImGui::Text("Admin User: ");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(600);
        ImGui::InputText("##admin_enter", adminname, USERLENGTH);

        ImGui::NewLine();
        ImGui::Text("Enter Regular Users:");
        ImGui::NewLine();
        ImGui::Text("Name:                                                    "
                    "     Permission (true = advanced, false = normal");

        int i = 0;
        for(const auto& entry : users) {
            ImGui::SetNextItemWidth(600);
            ImGui::InputText((ST::string("##usernameinput") + i).c_str(), entry.first, USERLENGTH);
            ImGui::SameLine();
            ImGui::Checkbox(((ST::string("##usercheckbox") + i).c_str()), entry.second);
            i++;
        }

        if(ImGui::Button("Add more users")) {
            char* newbuf = new char[USERLENGTH];
            memset((void*) newbuf, 0, USERLENGTH);
            bool* newbool = new bool(false);

            users[newbuf] = newbool;
        }

        ImGui::NewLine();
        if(ImGui::Button("Confirm Users")) {
            hasConfirmed = true;

            Json(config["keys"]) << Json::object();
            config["keys"][0]["name"] = adminname;
            config["keys"][0]["perm"] = 0;

            bool yes = true;
            users[adminname] = &yes;

            for(const auto& entry : users) {
                int length = Json(config["keys"]).size();
                Json(config["keys"]) << Json::object();
                config["keys"][length]["name"] = entry.first;
                config["keys"][length]["perm"] = *entry.second ? 1 : 2;
            }

            exec = new std::thread([this](){
                execDone = false;
                execProgress = 0;
                execTotal = users.size();

                for(const auto& entry : users) {
                    int rc = genSSHKeyToFile(("keys/" + ST::string(entry.first)).c_str());
                    std::cout << entry.first << ": " << rc << std::endl;
                    execProgress++;
                }

                execDone = true;
            });
        }
        else if(hasConfirmed) ImGui::EndDisabled();

        if(hasConfirmed) {
            ImGui::Text("Generating keys: %i / %llu", execProgress.load(), execTotal.load());

            if(!execDone) {
                ImGui::SameLine();
                ImGui::Spinner("##libs_extract", 10, 1,
                               ImGui::GetColorU32(ImVec4(0.1, 0.1, 0.75, 1)));
            }

            else {
                if(exec != nullptr) {
                    exec->join();
                    delete exec;
                    exec = nullptr;
                }

                ImGui::Text("Key Generation Complete");
            }
        }
    }
}