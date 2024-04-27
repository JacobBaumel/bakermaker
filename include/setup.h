//
// Created by skipp on 4/27/2024.
//

#ifndef BAKERMAKER_SETUP_H
#define BAKERMAKER_SETUP_H

#include "GLFW/glfw3.h"

namespace bakermaker {
    void init(GLFWwindow* window);
    void prerender();
    void postrender(GLFWwindow* window);
    void shutdown(GLFWwindow* window);
}

#endif //BAKERMAKER_SETUP_H