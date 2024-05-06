//
// Created by skipp on 4/27/2024.
//

#ifndef BAKERMAKER_SETUP_H
#define BAKERMAKER_SETUP_H

#include "GLFW/glfw3.h"
#include "string_theory/string"
#include "ImguiMarkdownRender.h"
#include "UI/BaseUIScreen.h"

namespace ST {
    string operator+(const string& str, int num);
}

namespace bakermaker {
    extern ImguiMarkdownRender* documentation;
    extern bakermaker::ProgramStage stage;
    extern ST::string documarkdown;

    void init(GLFWwindow* window);
    void prerender();
    void postrender(GLFWwindow* window);
    void shutdown(GLFWwindow* window);
}

#endif //BAKERMAKER_SETUP_H
