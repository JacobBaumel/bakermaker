//
// Created by skipp on 4/27/2024.
//

#ifndef BAKERMAKER_SETUP_H
#define BAKERMAKER_SETUP_H

#include "GLFW/glfw3.h"
#include "string_theory/string"
#include "ImguiMarkdownRender.h"
#include "UI/BaseUIScreen.h"
#include "json11.h"

namespace bakermaker {
    extern ImguiMarkdownRender* documentation;
    extern bakermaker::ProgramStage stage;
    extern ST::string documarkdown;
    extern Json config;

    void init(GLFWwindow* window);
    void prerender();
    void postrender(GLFWwindow* window);
    void shutdown(GLFWwindow* window);
}

#endif //BAKERMAKER_SETUP_H
