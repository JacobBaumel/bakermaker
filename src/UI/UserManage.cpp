#include "UI/UserManage.h"
#include <filesystem>
#include "utils.h"
#include <set>
#include <iostream>

namespace bakermaker {
    UserManage::UserManage() : BaseUIScreen(bakermaker::ProgramStage::USER_MANAGE,
        &bakermaker::configScreens), exec(nullptr), execDone(false), success(0) {
        if(!std::filesystem::exists("keys") && !std::filesystem::is_directory("keys"))
            std::filesystem::create_directories("keys");
    }

    void UserManage::render() {
        if(!config["synced"].get<bool>()) ImGui::BeginDisabled();

        ImGui::PushFont(fontlist[1]);
        ImGui::Text("Manage Users");
        ImGui::PopFont();
        ImGui::Separator();

        if(exec && !execDone) ImGui::BeginDisabled();

        ImGui::Text("Enter New User: ");
        ImGui::SameLine();
        ImGui::InputText("##newuser_enter", newName, USERLENGTH);
        ImGui::SameLine();

        if(ImGui::Button("Add")) {
            execDone = false;

            std::set<std::string> users = config["keys"].get<std::set<std::string>>();

            if(users.contains(std::string(newName))) {
                bakermaker::startErrorModal((ST::string("User \"") + newName + "\" has already been added.").c_str());
            }
            else {
                exec = new std::thread(&bakermaker::createUser, newName, &execDone, &success);
                ImGui::BeginDisabled();
            }
        }

        if(exec && !execDone) {
            ImGui::EndDisabled();
            ImGui::SameLine();
            if(!execDone) bakermaker::spinner();
        }

        if(execDone) {
            if(exec) {
                exec->join();
                delete exec;
                exec = nullptr;
                std::cout << "eeee" << std::endl;

                switch(success) {
                    case 0:
                        config["keys"].push_back(newName);
                        break;

                    case 1:
                        bakermaker::startErrorModal("Failed to generate SSH key!");
                        break;

                    case 2:
                        bakermaker::startErrorModal("Failed to export key to file!");
                        break;

                    case 3:
                        bakermaker::startErrorModal("Failed to generate public key!");
                        break;

                    case 4:
                        bakermaker::startErrorModal("Failed to export public key to file!");
                        break;
                }
            }

            if(success == 0) {
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0, 1, 0, 1));
                ImGui::Text("Success!");
                ImGui::PopStyleColor();
            }
        }

        if(ImGui::BeginTable("table", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg, ImVec2(600, 0))) {
            ImGui::TableSetupColumn("Name");
            ImGui::TableSetupColumn("Remove");
            ImGui::TableHeadersRow();

            ImGui::TableNextRow();

            ImGui::TableNextColumn();
            ImGui::TextUnformatted(config["keys"][0].get<std::string>().c_str());
            ImGui::TableNextColumn();
            ImGui::Text("Admin");

            for(int i = 1; i < config["keys"].size(); i++) {
                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::TextUnformatted(config["keys"][i].get<std::string>().c_str());
                ImGui::TableNextColumn();
                if(ImGui::Button((ST::string("Delete##") + std::to_string(i)).c_str())) {

                }
            }

            ImGui::EndTable();
        }

        if(!config["synced"].get<bool>()) ImGui::EndDisabled();
    }
}