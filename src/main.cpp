#include <iostream>

#include "GLFW/glfw3.h"
#include "setup.h"

int main() {
    if(!glfwInit()) {
        std::cout << "Error initing glfw" << std::endl;
        std::exit(-1);
    }

    GLFWwindow* window = glfwCreateWindow(1920, 1080, "Window", nullptr, nullptr);
    if(!window) {
        std::cout << "Error creating window!" << std::endl;
        std::exit(-1);
    }

    bakermaker::init(window);

    while(!glfwWindowShouldClose(window)) {
        bakermaker::prerender();

        bakermaker::postrender(window);
    }

    bakermaker::shutdown(window);

    return 0;
}