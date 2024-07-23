#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")

#include <iostream>

#include "GLFW/glfw3.h"
#include "setup.h"
#include "imgui.h"
#include "ImguiMarkdownRender.h"
#include "romfs/romfs.hpp"
#include "UI/BaseUIScreen.h"
#include "UI/ServerConnect.h"
#include "UI/LibsNotFound.h"
#include "utils.h"

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

    bool libsfound = true;
    bakermaker::LibsNotFound* lnf;
    {
        HMODULE ssh = LoadLibraryA("ssh.dll");
        if(ssh == nullptr) {
            libsfound = false;
            lnf = new bakermaker::LibsNotFound();
        }
        FreeLibrary(ssh);
    }

    while(!glfwWindowShouldClose(window)) {
        bakermaker::resetIds();
        bakermaker::prerender();

        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(ImGui::GetMainViewport()->Size);
        if(ImGui::Begin("Bakermaker", nullptr, flags)) {
            if(!libsfound) {
                lnf->render();
            }

            else {
                if(ImGui::BeginTabBar("##tabbar", tabflags)) {
                    bool setup = bakermaker::config["setup"].get<bool>();
                    if(ImGui::BeginTabItem((setup ? "Server Config" :
                            "Server Management"), nullptr, tabitemflags)) {
                        if(ImGui::BeginChild("##workchild")) {
                            for(auto& screen : (setup ? bakermaker::configScreens : bakermaker::setupScreens)) {
                                screen.second->render();
                                ImGui::NewLine();
                            }

                            bakermaker::displayErrorModal();
                            ImGui::EndChild();
                        }

                        ImGui::EndTabItem();
                    }
                    if(ImGui::BeginTabItem("Documentation", nullptr, tabitemflags)) {
                        if(ImGui::BeginChild("##docchild")) {
                            ImGui::Dummy(ImVec2(0, 15));
                            ImGui::SetNextItemWidth(ImGui::GetIO().DisplaySize.x / 1.5);
                            bakermaker::documentation->render(bakermaker::documarkdown);
                            ImGui::EndChild();
                        }
                        ImGui::EndTabItem();
                    }

                    ImGui::EndTabBar();
                }
            }
        }

        ImGui::End();

        bakermaker::postrender(window);
    }

    bakermaker::shutdown(window);

    return 0;
}