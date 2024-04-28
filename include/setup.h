//
// Created by skipp on 4/27/2024.
//

#ifndef BAKERMAKER_SETUP_H
#define BAKERMAKER_SETUP_H

#include "GLFW/glfw3.h"
#include "string_theory/string"

namespace ST {
    string operator+(const string& str, int num);
}

namespace bakermaker {
    static int latestId;
    static const char* appendId(ST::string string);
    static const char* appendId(const char*);
    static void resetIds();

    void init(GLFWwindow* window);
    void prerender();
    void postrender(GLFWwindow* window);
    void shutdown(GLFWwindow* window);
}

#endif //BAKERMAKER_SETUP_H
