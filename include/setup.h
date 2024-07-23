#ifndef BAKERMAKER_SETUP_H
#define BAKERMAKER_SETUP_H

#include "GLFW/glfw3.h"
#include "ImguiMarkdownRender.h"
#include "STStringJson.h"
#include "nlohmann/json.hpp"

namespace bakermaker {
    extern ImguiMarkdownRender* documentation;
    extern ST::string documarkdown;
    extern nlohmann::json config;

    // Init function to set up glfw window and context
    void init(GLFWwindow* window);

    // ImGui code to run before running UI code
    void prerender();

    // ImGui code to run after UI code
    void postrender(GLFWwindow* window);

    // Shutdown code
    void shutdown(GLFWwindow* window);
}

#endif //BAKERMAKER_SETUP_H
