#include <charconv>
#include "setup.h"

#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_glfw.h"
#include "string_theory/string"
#include "STStringJson.h"
#include "romfs/romfs.hpp"
#include <fstream>
#include "nlohmann/json.hpp"
#include "stb_image.h"

#include "UI/BaseUIScreen.h"
#include "UI/ServerSetup.h"
#include "UI/ServerConnect.h"
#include "UI/ServerInstall.h"
#include "UI/CreateAdminKey.h"
#include "UI/SyncToServer.h"
#include "UI/UserManage.h"
#include "UI/RepoManage.h"
#include "UI/ClientExtract.h"

namespace bakermaker {
    bakermaker::ImguiMarkdownRender* documentation;
    bakermaker::ProgramStage stage = bakermaker::ProgramStage::SERVER_CONNECT;
    ST::string documarkdown;
    nlohmann::json config;

    void init(GLFWwindow* window) {
        glfwMakeContextCurrent(window);
        glfwSwapInterval(1);
        glfwSetWindowTitle(window, "Gitolite Server Setup");

        {
            int imagew = 0;
            int imageh = 0;
            romfs::Resource im = romfs::get("icon.png");
            GLFWimage images[1];
            images[0].pixels = stbi_load_from_memory((unsigned char*) im.data(), im.size(),
                                                     &imagew, &imageh, nullptr, 4);
            images[0].height = imageh;
            images[0].width = imagew;
            glfwSetWindowIcon(window, 1, images);
            stbi_image_free(images[0].pixels);
        }

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
            config = nlohmann::json::parse(jsonFile);
        }

        else {
            config = {
                    {"synced", false},
                    {"extracting", false},
                    {"unsaved", false},
                    {"setup", false},
                    {"iscsi", {"", "", ""}},
                    {"useiscsi", false},
                    {"server", {
                            {"ip", ""},
                            {"port", 22},
                            {"user", "ubuntu"},
                            {"keyfile", ""}}},
                    {"keys", nlohmann::json::array()}
            };
        }

        new bakermaker::ServerSetup();
        new bakermaker::ServerConnect();
        new bakermaker::ServerInstall();
        new bakermaker::CreateAdminKey();
        new bakermaker::SyncToServer();
        new bakermaker::UserManage();
        new bakermaker::RepoManage();
        new bakermaker::ClientExtract();
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
            jsonFile << config.dump(4);
        }

        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();

        glfwDestroyWindow(window);
        glfwTerminate();
    }

}