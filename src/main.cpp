#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")

#include <Windows.h>

#include "GLFW/glfw3.h"
#include "imgui.h"
#include "ImguiMarkdownRender.h"

#include "UI/BaseUIScreen.h"
#include "UI/LibsNotFound.h"
#include "setup.h"
#include "utils.h"

static constexpr ImGuiWindowFlags flags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove |
    ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoDecoration;
static constexpr ImGuiTabBarFlags tabflags = ImGuiTabBarFlags_NoCloseWithMiddleMouseButton | ImGuiTabBarFlags_NoTooltip;
static constexpr ImGuiTabItemFlags tabitemflags = ImGuiTabItemFlags_NoCloseWithMiddleMouseButton |
    ImGuiTabItemFlags_NoReorder;

int main() {
    if(!glfwInit()) {
        std::exit(-1);
    }

    glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);
    GLFWwindow* window = glfwCreateWindow(1920, 1080, "Bakermaker - Git Server Management", nullptr, nullptr);
    if(!window) {
        std::exit(-1);
    }

    bakermaker::init(window);

    // Test load the ssh library to see if openssl and ssh dll's need to be extracted
    bakermaker::LibsNotFound* lnf = nullptr;

    if(const HMODULE ssh = LoadLibraryA("ssh.dll"); ssh == nullptr) {
        lnf = new bakermaker::LibsNotFound();
    }
    else FreeLibrary(ssh);

    while(!glfwWindowShouldClose(window)) {
        bakermaker::prerender();

        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(ImGui::GetMainViewport()->Size);
        if(ImGui::Begin("Bakermaker", nullptr, flags)) {
            if(lnf) lnf->render();
            else {
                if(ImGui::BeginTabBar("##tabbar", tabflags)) {
                    bool setup = bakermaker::config["setup"].get<bool>();
                    if(ImGui::BeginTabItem((setup ? "Server Config" : "Server Management"), nullptr, tabitemflags)) {
                        if(ImGui::BeginChild("##workchild")) {
                            for(auto& screen :
                                (setup ? bakermaker::configScreens : bakermaker::setupScreens)) {
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
                            bakermaker::renderDocs();
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
