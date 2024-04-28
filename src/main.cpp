#include <iostream>

#include "GLFW/glfw3.h"
#include "setup.h"
#include "imgui.h"

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

    while(!glfwWindowShouldClose(window)) {
        bakermaker::latestId = 0;
        bakermaker::prerender();

        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(ImGui::GetMainViewport()->Size);
        if(ImGui::Begin("Bakermaker", &open, flags)) {
            if(ImGui::BeginTabBar("##tabbar", tabflags)) {
                if(ImGui::BeginTabItem("Item 1", &open1, tabitemflags)) {
                    ImGui::EndTabItem();
                }
                if(ImGui::BeginTabItem("Item 2", &open2, tabitemflags)) {
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