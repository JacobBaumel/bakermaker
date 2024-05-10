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

    bool open = true, open1 = true, open2 = true;


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
        if(ImGui::Begin("Bakermaker", &open, flags)) {
            if(!libsfound) {
                lnf->render();
            }

            else {
                if(ImGui::BeginTabBar("##tabbar", tabflags)) {
                    if(ImGui::BeginTabItem("Server Management", &open1, tabitemflags)) {
                        if(ImGui::Button("error")) bakermaker::startErrorModal("this is a test error");
                        for(auto& screen : bakermaker::screens) {
                            screen.second->render();
                            ImGui::NewLine();
                        }

                        bakermaker::displayErrorModal();
                        ImGui::EndTabItem();
                    }
                    if(ImGui::BeginTabItem("Documentation", &open2, tabitemflags)) {
                        ImGui::SetNextItemWidth(ImGui::GetIO().DisplaySize.x / 1.5);
                        bakermaker::documentation->render(bakermaker::documarkdown);
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