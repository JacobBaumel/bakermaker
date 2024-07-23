#include <chrono>
#include <filesystem>
#include <set>

#include "romfs/romfs.hpp"

#include "UI/BaseUIScreen.h"
#include "UI/CreateAdminKey.h"
#include "setup.h"
#include "ssh_helper.h"
#include "utils.h"

namespace bakermaker {
    CreateAdminKey::CreateAdminKey() : BaseUIScreen(ProgramStage::SSH_KEYGEN_ADMIN, &setupScreens), showSuccess(false) {
        // Load instructions from romfs
        const romfs::Resource text = romfs::get("SSHAdminKeyGen.md");
        instructions = ST::string((const char*)text.data(), text.size());

        // Create "keys/" directory if it does not exist
        if(!std::filesystem::exists("keys") && !std::filesystem::is_directory("keys"))
            std::filesystem::create_directories("keys");
    }

    void CreateAdminKey::render() {
        // Section header
        ImGui::PushFont(fontlist[1]);
        ImGui::Text("Create SSH Keys");
        ImGui::PopFont();
        ImGui::Separator();

        // Render instructions
        documentation->render(instructions);
        ImGui::NewLine();

        // If button pushed, create the admin key file
        ImGui::Text("Create admin user: ");
        if(ImGui::Button("Create Admin User")) {
            if(config["keys"].get<std::set<std::string>>().contains(std::string("admin"))) {
                startErrorModal("User admin has already been added.");
            }

            else
                switch(genSSHKeyToFile("keys/admin")) {
                case 0:
                    config["keys"].push_back("admin");
                    showSuccess = true;
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
                    break;
                }
        }

        if(showSuccess) {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0, 1, 0, 1));
            ImGui::Text("Success!");
            ImGui::PopStyleColor();
        }
    }
}
