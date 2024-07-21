#include "UI/UserManage.h"
#include "UI/RepoManage.h"
#include <filesystem>
#include "utils.h"
#include "ssh_helper.h"
#include <set>
#include <iostream>

namespace bakermaker {
    UserManage::UserManage() : BaseUIScreen(bakermaker::ProgramStage::USER_MANAGE,
                                            &bakermaker::configScreens) {
        if(!std::filesystem::exists("keys") && !std::filesystem::is_directory("keys"))
            std::filesystem::create_directories("keys");
    }

    void UserManage::render() {
        using namespace ST::literals;

        if(!config["synced"].get<bool>() || config["extracting"].get<bool>())
            ImGui::BeginDisabled();

        ImGui::PushFont(fontlist[1]);
        ImGui::Text("Manage Users");
        ImGui::PopFont();
        ImGui::Separator();

        ImGui::Text("Enter New User: ");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(600);
        bool enter = ImGui::InputText("##newuser_enter", newName, USERLENGTH,
                                      ImGuiInputTextFlags_EnterReturnsTrue);
        ImGui::SameLine();

        if(enter || ImGui::Button("Add")) {
            if(newName[0] == '\0') bakermaker::startErrorModal("Please enter a name!");

            if(config["keys"].get<std::set<std::string>>().contains(std::string(newName))) {
                bakermaker::startErrorModal(
                        (ST::string("User \"") + newName + "\" has already been added.").c_str());
            }
            else {
                switch(-genSSHKeyToFile(("keys/"_st + newName).c_str())) {
                    case 0:
                        config["keys"].push_back(newName);
                        memset((void*) newName, 0, USERLENGTH);
                        config["unsaved"] = true;
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
                if(enter) ImGui::SetKeyboardFocusHere(-1);
            }
        }

        if(ImGui::BeginTable("table", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg,
                             ImVec2(600, 0))) {
            ImGui::TableSetupColumn("Name");
            ImGui::TableSetupColumn("Remove");
            ImGui::TableHeadersRow();

            ImGui::TableNextRow();

            ImGui::TableNextColumn();
            ImGui::Text("admin");
            ImGui::TableNextColumn();
            ImGui::Text("Admin");

            for(int i = 1; i < config["keys"].size(); i++) {
                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::TextUnformatted(config["keys"][i].get<ST::string>().c_str());
                ImGui::TableNextColumn();
                if(ImGui::Button((ST::string("Delete##") + std::to_string(i)).c_str())) {
                    ST::string user = config["keys"][i].get<ST::string>();
                    std::filesystem::remove(("keys/"_st + user + ".pub"_st).c_str());
                    std::filesystem::remove(("keys/"_st + user).c_str());
                    ((bakermaker::RepoManage*) bakermaker::configScreens[bakermaker::ProgramStage::REPO_MANAGE])->deleteUser(
                            user);
                    config["keys"].erase(i);
                    config["unsaved"] = true;
                }
            }

            ImGui::EndTable();
        }

        if(!config["synced"].get<bool>() || config["extracting"].get<bool>()) ImGui::EndDisabled();
    }
}