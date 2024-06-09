#include <charconv>
#include "setup.h"

#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_glfw.h"
#include "string_theory/string"
#include "romfs/romfs.hpp"
#include "json11.h"
#include <fstream>

#include "UI/BaseUIScreen.h"
#include "UI/ServerSetup.h"
#include "UI/ServerConnect.h"
#include "UI/ServerInstall.h"
#include "UI/CreateSSHKeys.h"
#include "UI/RepoManage.h"

namespace bakermaker {
    bakermaker::ImguiMarkdownRender* documentation;
    bakermaker::ProgramStage stage = bakermaker::ProgramStage::SERVER_CONNECT;
    ST::string documarkdown;
    Json config;

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
            ImFontConfig fontConfig;
            fontConfig.FontDataOwnedByAtlas = false;
            io.Fonts->Clear();
            romfs::Resource umr = romfs::get("font-regular.ttf");
            fonts[0] = io.Fonts->AddFontFromMemoryTTF((void *) umr.data(), umr.size(), 15, &fontConfig);
            umr = romfs::get("font-bold.ttf");
            fonts[1] = io.Fonts->AddFontFromMemoryTTF((void *) umr.data(), umr.size(), 25, &fontConfig);
            fonts[2] = io.Fonts->AddFontFromMemoryTTF((void *) umr.data(), umr.size(), 20, &fontConfig);
            fonts[3] = io.Fonts->AddFontFromMemoryTTF((void *) umr.data(), umr.size(), 18, &fontConfig);
        }

        bakermaker::fontlist = fonts;

        romfs::Resource mdfile = romfs::get("Documentation.md");
        documarkdown = ST::string((char *) mdfile.data(), mdfile.size());
        bakermaker::documentation = new ImguiMarkdownRender(bakermaker::fontlist + 1);

        if(std::filesystem::exists("config.json")) {
            std::ifstream jsonFile("config.json");
            jsonFile >> config;
        }

        else {
            config["iscsi"] = {"", "", ""};
            Json server;
            server.set("ip", "").set("port", 22).set("user", "ubuntu").set("keyfile", "");
            config["server"] = server;
            config["keys"] = Json::array();
        }

        new bakermaker::ServerSetup();
        new bakermaker::ServerConnect();
        new bakermaker::ServerInstall();
        new bakermaker::CreateSSHKeys();
        new bakermaker::RepoManage();
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

        {
            std::ofstream jsonFile("config.json");
            jsonFile << config;
        }

        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();

        glfwDestroyWindow(window);
        glfwTerminate();
    }

}