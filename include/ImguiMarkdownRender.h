#ifndef BAKERMAKER_IMGUIMARKDOWNRENDER_H
#define BAKERMAKER_IMGUIMARKDOWNRENDER_H

#include <map>

#include "GLFW/glfw3.h"
#include "immarkdown.h"
#include "string_theory/string"

namespace bakermaker {
    // Wrapper for the markdown renderer
    class ImguiMarkdownRender {
        static void linkCallback(ImGui::MarkdownLinkCallbackData data);
        static ImGui::MarkdownImageData imageCallback(ImGui::MarkdownLinkCallbackData callbackData);
        static bool loadTextureFromRomfs(const char* filename, GLuint* tex, int* width, int* height);
        ImGui::MarkdownConfig config;

        std::map<ST::string, ImGui::MarkdownImageData> images;
        std::vector<GLuint> imageNames;

    public:
        explicit ImguiMarkdownRender(ImFont** headerFonts);
        void render(const ST::string& markdown);
        void render(ST::string* markdown);
    };
}

#endif //BAKERMAKER_IMGUIMARKDOWNRENDER_H
