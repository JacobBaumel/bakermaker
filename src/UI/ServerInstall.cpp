#include "UI/ServerInstall.h"
#include "romfs/romfs.hpp"
#include "setup.h"

namespace bakermaker {
    ServerInstall::ServerInstall() : BaseUIScreen(bakermaker::ProgramStage::SERVER_INSTALL) {
        romfs::Resource md = romfs::get("ServerInstallText.md");
        instructions = ST::string((const char*) md.data(), md.size());
    }

    void ServerInstall::render(bakermaker::ProgramStage &stage) {
        ImGui::PushFont(fontlist[1]);
        ImGui::Text("Server Software Install");
        ImGui::PopFont();
        ImGui::Separator();

        bakermaker::documentation->render(instructions);

        ImGui::NewLine();
        ImGui::Button("Begin");
    }
}