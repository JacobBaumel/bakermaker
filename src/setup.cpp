#include <charconv>
#include "setup.h"

#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_glfw.h"
#include "string_theory/string"

namespace ST {
    string operator+(const string& str, int num) {
        int digits = 1;
        int numcopy = num;
        while(numcopy / 10 > 0) {
            numcopy /= 10;
            digits++;
        }

        char charnum[digits];

        for(int i = 0; i < digits; i++) {
            charnum[digits - i - 1] = '0' + (num % 10);
            num /= 10;
        }

        ST::char_buffer cat;
        cat.allocate(str.size() + digits);
        std::char_traits<char>::copy(&cat[0], str.c_str(), str.size());
        std::char_traits<char>::copy(&cat[str.size()], charnum, digits);

        return string::from_validated(std::move(cat));
    }
}

namespace bakermaker {
    static const char* appendId(::ST::string string) {
        return (string + latestId).c_str();
    }

    static const char* appendId(const char* str) {
        return appendId(ST::string(str));
    }

    static void resetIds() {
        latestId = 0;
    }

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
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();

        glfwDestroyWindow(window);
        glfwTerminate();
    }

}