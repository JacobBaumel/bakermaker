//
// Created by skipp on 4/27/2024.
//

#ifndef BAKERMAKER_IMGUIMARKDOWNRENDER_H
#define BAKERMAKER_IMGUIMARKDOWNRENDER_H

#include "immarkdown.h"

namespace bakermaker {
    class ImguiMarkdownRender {
    private:
        static void linkCallback(ImGui::MarkdownLinkCallbackData data);
        static ImGui::MarkdownImageData imageCallback(ImGui::MarkdownLinkCallbackData data);
        ImGui::MarkdownConfig config;

    public:
        ImGuiMark
    };
}

#endif //BAKERMAKER_IMGUIMARKDOWNRENDER_H
