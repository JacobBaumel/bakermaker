//
// Created by skipp on 4/27/2024.
//

#ifndef BAKERMAKER_IMGUIMARKDOWNRENDER_H
#define BAKERMAKER_IMGUIMARKDOWNRENDER_H

#include "immarkdown.h"
#include <map>
#include "GLFW/glfw3.h"
#include "string_theory/string"

namespace bakermaker {
    class ImguiMarkdownRender {
    private:
        static void linkCallback(ImGui::MarkdownLinkCallbackData data);
        static ImGui::MarkdownImageData imageCallback(ImGui::MarkdownLinkCallbackData data);
        static bool loadTextureFromRomfs(const char* filename, GLuint* tex, int* width, int* height);
        ImGui::MarkdownConfig config;

        std::map<ST::string, ImGui::MarkdownImageData> images;
        std::vector<GLuint> imageNames;

    public:
        explicit ImguiMarkdownRender(ImFont** headerFonts);
        ~ImguiMarkdownRender();
        void render(const ST::string& markdown);
    };
}

#endif //BAKERMAKER_IMGUIMARKDOWNRENDER_H
