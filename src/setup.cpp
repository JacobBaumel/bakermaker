#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")

#include <charconv>
#include "setup.h"

#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_glfw.h"
#include "string_theory/string"
#include "romfs/romfs.hpp"
#include "UI/BaseUIScreen.h"
#include "UI/ServerConnect.h"

namespace bakermaker {
    bakermaker::ImguiMarkdownRender* documentation;
    bakermaker::ProgramStage stage = bakermaker::ProgramStage::SERVER_CONNECT;

    void init(GLFWwindow* window) {
        glfwMakeContextCurrent(window);
        glfwSwapInterval(1);

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
        io.IniFilename = nullptr;

        ImGui::StyleColorsDark();

        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init();

        ImFont **fonts = new ImFont *[4];
        {
            ImFontConfig config;
            config.FontDataOwnedByAtlas = false;
            io.Fonts->Clear();
            romfs::Resource umr = romfs::get("UbuntuMono-Regular.ttf");
            fonts[0] = io.Fonts->AddFontFromMemoryTTF((void *) umr.data(), umr.size(), 15, &config);
            umr = romfs::get("UbuntuMono-Bold.ttf");
            fonts[1] = io.Fonts->AddFontFromMemoryTTF((void *) umr.data(), umr.size(), 25, &config);
            fonts[2] = io.Fonts->AddFontFromMemoryTTF((void *) umr.data(), umr.size(), 20, &config);
            fonts[3] = io.Fonts->AddFontFromMemoryTTF((void *) umr.data(), umr.size(), 18, &config);
        }

        bakermaker::fontlist = fonts;

        ST::string markdown;
        romfs::Resource mdfile = romfs::get("test.md");
        markdown = ST::string((char *) mdfile.data(), mdfile.size());
        bakermaker::documentation = new ImguiMarkdownRender(markdown, bakermaker::fontlist + 1);

        new bakermaker::ServerConnect();
    }

    void prerender() {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }

    void postrender(GLFWwindow* window) {
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    void shutdown(GLFWwindow *window) {
        delete bakermaker::documentation;

        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();

        glfwDestroyWindow(window);
        glfwTerminate();
    }

}