#include "immarkdown.h"
#include "ImguiMarkdownRender.h"
#include <Windows.h>
#include <iostream>

namespace bakermaker {
    void ImguiMarkdownRender::linkCallback(ImGui::MarkdownLinkCallbackData data) {
        if(!data.isImage) {
            ShellExecuteA(nullptr, "open", data.link, nullptr, nullptr, SW_SHOWNORMAL);
        }
    }

    ImGui::MarkdownImageData ImguiMarkdownRender::imageCallback(ImGui::MarkdownLinkCallbackData data) {
        std::cout << "data link: " << data.link << "\ndata link length: " << data.linkLength
            << "\ndata text: " << data.text << "\ndata text length: " << data.textLength << std::endl;
        ImGui::MarkdownImageData imageData;
        return imageData;
    }
}