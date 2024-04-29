#include <iostream>

#include "GLFW/glfw3.h"
#include "setup.h"
#include "imgui.h"
#include "ImguiMarkdownRender.h"
#include "romfs/romfs.hpp"
#include "UI/BaseUIScreen.h"
#include "UI/ServerConnect.h"

int main() {
    if(!glfwInit()) {
        std::cout << "Error initing glfw" << std::endl;
        std::exit(-1);
    }

    glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);

    GLFWwindow* window = glfwCreateWindow(1920, 1080, "Bakermaker - Git Server Management", nullptr, nullptr);
    if(!window) {
        std::cout << "Error creating window!" << std::endl;
        std::exit(-1);
    }

    bakermaker::init(window);
    const ImGuiWindowFlags flags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse |
                                   ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoDecoration;

    const ImGuiTabBarFlags tabflags = ImGuiTabBarFlags_NoCloseWithMiddleMouseButton | ImGuiTabBarFlags_NoTooltip;
    const ImGuiTabItemFlags tabitemflags = ImGuiTabItemFlags_NoCloseWithMiddleMouseButton | ImGuiTabItemFlags_NoReorder;

    bool open = true, open1 = true, open2 = true;

    ImGuiIO& io = ImGui::GetIO();
    ImFont** fonts = new ImFont*[4];
    {
        ImFontConfig config;
        config.FontDataOwnedByAtlas = false;
        io.Fonts->Clear();
        romfs::Resource umr = romfs::get("UbuntuMono-Regular.ttf");
        fonts[0] = io.Fonts->AddFontFromMemoryTTF((void *) umr.data(), umr.size(), 15, &config);
        umr = romfs::get("UbuntuMono-Bold.ttf");
        fonts[1] = io.Fonts->AddFontFromMemoryTTF((void*) umr.data(), umr.size(), 20, &config);
        fonts[2] = io.Fonts->AddFontFromMemoryTTF((void*) umr.data(), umr.size(), 18, &config);
        fonts[3] = io.Fonts->AddFontFromMemoryTTF((void*) umr.data(), umr.size(), 16, &config);
    }

    bakermaker::fontlist = fonts;

    ST::string markdown;
    {
        romfs::Resource mdfile = romfs::get("test.md");
        markdown = ST::string((char*) mdfile.data(), mdfile.size());
    }
    bakermaker::ImguiMarkdownRender testtext(markdown, fonts + 1);

    new bakermaker::ServerConnect();

    bakermaker::ProgramStage stage = bakermaker::ProgramStage::SERVER_CONNECT;

    while(!glfwWindowShouldClose(window)) {
        bakermaker::resetIds();
        bakermaker::prerender();

        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(ImGui::GetMainViewport()->Size);
        if(ImGui::Begin("Bakermaker", &open, flags)) {
            if(ImGui::BeginTabBar("##tabbar", tabflags)) {
                if(ImGui::BeginTabItem("Server Management", &open1, tabitemflags)) {
//                    ImGui::BeginDisabled();
                    for(auto& screen : bakermaker::screens) {
//                        if(screen.first == stage) ImGui::EndDisabled();
                        screen.second->render(stage);
//                        if(screen.first == stage) ImGui::BeginDisabled();
                    }

//                    ImGui::EndDisabled();
                    ImGui::EndTabItem();
                }
                if(ImGui::BeginTabItem("Documentation", &open2, tabitemflags)) {
                    testtext.render();
                    ImGui::EndTabItem();
                }

                ImGui::EndTabBar();
            }
        }

        ImGui::End();

        bakermaker::postrender(window);
    }

    bakermaker::shutdown(window);

    return 0;
}