#include <charconv>
#include <fstream>

#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_glfw.h"
#include "stb_image.h"
#include "string_theory/string"
#include "STStringJson.h"
#include "nlohmann/json.hpp"

#include "WindowsResource.h"
#include "setup.h"
#include "UI/BaseUIScreen.h"
#include "UI/ServerSetup.h"
#include "UI/ServerConnect.h"
#include "UI/ServerInstall.h"
#include "UI/CreateAdminKey.h"
#include "UI/SyncToServer.h"
#include "UI/UserManage.h"
#include "UI/RepoManage.h"
#include "UI/ClientExtract.h"
#include "UI/UsedStorage.h"
#include "UI/DebugExport.h"

namespace bakermaker {
    ImguiMarkdownRender* documentation;
    nlohmann::json config;

    void init(GLFWwindow* window) {
        glfwMakeContextCurrent(window);
        glfwSwapInterval(1);
        glfwSetWindowTitle(window, "Gitolite Server Setup");

        // Load window icon
        {
            int imagew = 0;
            int imageh = 0;
            WindowsResource im("images/icon.png", "PNGIMG");
            GLFWimage images[1];
            images[0].pixels = stbi_load_from_memory((unsigned char*)im.getData(), im.getSize(),
                                                     &imagew, &imageh, nullptr, 4);
            images[0].height = imageh;
            images[0].width = imagew;
            glfwSetWindowIcon(window, 1, images);
            stbi_image_free(images[0].pixels);
        }

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad; // Enable Gamepad Controls
        io.IniFilename = nullptr;

        ImGui::StyleColorsDark();

        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init();

        // Load fonts
        ImFont** fonts = new ImFont*[4];
        {
            ImFontConfig fontConfig;
            fontConfig.FontDataOwnedByAtlas = false;
            io.Fonts->Clear();
            WindowsResource umr("font-regular.ttf", "TTFFONT");
            fonts[0] = io.Fonts->AddFontFromMemoryTTF((void*)umr.getData(), umr.getSize(), 15, &fontConfig);
            umr = WindowsResource("font-bold.ttf", "TTFFONT");
            fonts[1] = io.Fonts->AddFontFromMemoryTTF((void*)umr.getData(), umr.getSize(), 25, &fontConfig);
            fonts[2] = io.Fonts->AddFontFromMemoryTTF((void*)umr.getData(), umr.getSize(), 20, &fontConfig);
            fonts[3] = io.Fonts->AddFontFromMemoryTTF((void*)umr.getData(), umr.getSize(), 18, &fontConfig);
        }

        fontlist = fonts;

        documentation = new ImguiMarkdownRender(fontlist + 1);

        // If config.json exists, load it. If not, create a new one
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
                {
                    "server", {
                        {"ip", ""},
                        {"port", 22},
                        {"user", "ubuntu"}
                    }
                },
                {"keys", nlohmann::json::array()}
            };
        }

        // Create instances of UI elements
        new ServerSetup();
        new ServerConnect();
        new ServerInstall();
        new CreateAdminKey();
        new SyncToServer();
        new UserManage();
        new RepoManage();
        new ClientExtract();
        new UsedStorage();
        new DebugExport();
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

    void shutdown(GLFWwindow* window) {
        delete documentation;

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

    static void renderHeader(const ST::string& name, const ST::string& path, ST::string* doc) {
        if(ImGui::CollapsingHeader(name.c_str())) {
            if(!doc) {
                WindowsResource md(path.c_str(), "MARKDOWN");
                doc = new ST::string(md.getData(), md.getSize());
            }

            documentation->render(doc);
            ImGui::Separator();
        }

        else delete doc;
    }

    void renderDocs() {
        static ST::string* docs[4];

        renderHeader("Server Creation", "docs/ServerCreation.md", docs[0]);
        renderHeader("Server Installation", "docs/ServerSetup.md", docs[1]);
        renderHeader("Usage", "docs/Usage.md", docs[2]);
        renderHeader("Error Codes", "docs/Errors.md", docs[3]);
    }
}
