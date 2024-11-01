#include <filesystem>
#include <set>
#include <regex>

#include "UI/RepoManage.h"
#include "ssh_helper.h"
#include "UI/UserManage.h"
#include "utils.h"

namespace bakermaker {
    using namespace ST::literals;

    UserManage::UserManage() : BaseUIScreen(ProgramStage::USER_MANAGE, &configScreens) {
        // If "keys/" directory does not exist, create it
        if(!std::filesystem::exists("keys") && !std::filesystem::is_directory("keys"))
            std::filesystem::create_directories("keys");
    }

    void UserManage::render() {
        // Disable if first sync has not yet been done
        if(!config["synced"].get<bool>() || config["extracting"].get<bool>()) ImGui::BeginDisabled();

        // Header
        ImGui::PushFont(fontlist[1]);
        ImGui::Text("Manage Users");
        ImGui::PopFont();
        ImGui::Separator();

        ImGui::Text("Enter New User: ");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(600);
        bool enter = ImGui::InputText("##newuser_enter", newName, USERLENGTH, ImGuiInputTextFlags_EnterReturnsTrue);
        ImGui::SameLine();

        if(enter || ImGui::Button("Add")) {
            if(newName[0] == '\0') startErrorModal("Please enter a name!");

            else if(std::regex_search(
                newName, std::regex(R"([\(\)\{\}\[\]" "\@\#\$\%\^\&\*\!\.\,\/\\\<\>\;\:\'\"\`\~\-\+\=\|])"))) {
                startErrorModal(
                    R"(Name cannot contain one of the following characters: [](){}/\|,.<>?;:'"`~!@#$%^&*- or space)");
            }

            else if(config["keys"].get<std::set<std::string>>().contains(std::string(newName))) {
                startErrorModal(("User \""_st + newName + "\" has already been added."_st).c_str());
            }

            else {
                switch(-genSSHKeyToFile(("keys/"_st + newName).c_str())) {
                case 0:
                    config["keys"].push_back(newName);
                    memset(newName, 0, USERLENGTH);
                    config["unsaved"] = true;
                    break;

                case 1:
                    startErrorModal("Failed to generate SSH key!");
                    break;

                case 2:
                    startErrorModal("Failed to export key to file!");
                    break;

                case 3:
                    startErrorModal("Failed to generate public key!");
                    break;

                case 4:
                    startErrorModal("Failed to export public key to file!");
                    break;

                default:
                    startErrorModal("Unknown error code!");
                    break;
                }

                if(enter) ImGui::SetKeyboardFocusHere(-1);
            }
        }

        // Table to show all users
        if(ImGui::BeginTable("table", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg, ImVec2(600, 0))) {
            ImGui::TableSetupColumn("Name");
            ImGui::TableSetupColumn("Remove");
            ImGui::TableHeadersRow();

            ImGui::TableNextRow();

            ImGui::TableNextColumn();
            ImGui::Text("admin");
            ImGui::TableNextColumn();
            ImGui::Text("Admin");

            for(int i = 1; i < config["keys"].size(); i++) {
                ST::string user = config["keys"][i].get<ST::string>();
                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::TextUnformatted(user.c_str());
                ImGui::TableNextColumn();
                if(ImGui::Button(("Delete##"_st + std::to_string(i)).c_str())) {
                    std::filesystem::remove(("keys/"_st + user + ".pub"_st).c_str());
                    std::filesystem::remove(("keys/"_st + user).c_str());
                    ((RepoManage*)configScreens[ProgramStage::REPO_MANAGE])->deleteUser(user);
                    config["keys"].erase(i);
                    config["unsaved"] = true;
                }
            }

            ImGui::EndTable();
        }

        if(!config["synced"].get<bool>() || config["extracting"].get<bool>()) ImGui::EndDisabled();
    }
}
