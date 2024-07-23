#include <Windows.h>

#include "immarkdown.h"
#include "romfs/romfs.hpp"

#include "ImguiMarkdownRender.h"
#include "stb_image.h"

namespace bakermaker {
    void ImguiMarkdownRender::linkCallback(ImGui::MarkdownLinkCallbackData data) {
        if(!data.isImage) {
            // Open supplied link with default browser
            ST::string link(data.link, data.linkLength);
            ShellExecuteA(nullptr, "open", link.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
        }
    }

    ImGui::MarkdownImageData ImguiMarkdownRender::imageCallback(ImGui::MarkdownLinkCallbackData callbackData) {
        // Callback for markdown image rendering. Will load the appropriate image from romfs if it is not already
        // uploaded to the GPU.
        std::map<ST::string, ImGui::MarkdownImageData>& imageMap = ((ImguiMarkdownRender*)callbackData.userData)->images;
        ST::string imageName(callbackData.link, callbackData.linkLength);

        if(!imageMap.contains(imageName)) {
            ImGui::MarkdownImageData idata;
            idata.useLinkCallback = false;

            GLuint texture;
            int width;
            int height;
            bool success = loadTextureFromRomfs(imageName.c_str(), &texture, &width, &height);

            if(success) {
                ((ImguiMarkdownRender*)callbackData.userData)->imageNames.push_back(texture);
                idata.isValid = true;
                idata.user_texture_id = (void*)((long long)texture);
                idata.size = ImVec2((float)width / 2, (float)height / 2);
            }

            else {
                idata.isValid = false;
            }

            imageMap[imageName] = idata;
        }

        if(imageMap[imageName].isValid) {
            const ImVec2 contentSize = ImGui::GetContentRegionAvail();
            ImVec2 imageSize(imageMap[imageName].size);
            if(imageMap[imageName].size.x > contentSize.x) {
                float const ratio = imageSize.y / imageSize.x;
                imageSize.x = contentSize.x;
                imageSize.y = contentSize.x * ratio;
            }

            ImGui::MarkdownImageData data(imageMap[imageName]);
            data.size = imageSize;
            return data;
        }

        return imageMap[imageName];
    }

    bool ImguiMarkdownRender::loadTextureFromRomfs(const char* filename, GLuint* tex, int* width, int* height) {
        // Loads image from romfs and parses with stb_image, then uploads to the gpu for later use
        int image_width = 0;
        int image_height = 0;
        romfs::Resource image = romfs::get(filename);
        unsigned char* image_data = stbi_load_from_memory((unsigned char*)image.data(), image.size(),
                                                          &image_width, &image_height, nullptr, 4);
        if(image_data == nullptr)
            return false;

        // Create a OpenGL texture identifier
        GLuint image_texture;
        glGenTextures(1, &image_texture);
        glBindTexture(GL_TEXTURE_2D, image_texture);

        // Setup filtering parameters for display
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
#define GL_CLAMP_TO_EDGE 0x812F
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
#undef GL_CLAMP_TO_EDGE

        // Upload pixels to texture
#if defined(GL_UNPACK_ROW_LENGTH) && !defined(__EMSCRIPTEN__)
        glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
#endif
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height,
                     0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
        stbi_image_free(image_data);

        *tex = image_texture;
        *width = image_width;
        *height = image_height;

        return true;
    }

    ImguiMarkdownRender::ImguiMarkdownRender(ImFont** headerFonts) {
        config.linkCallback = &ImguiMarkdownRender::linkCallback;
        config.tooltipCallback = nullptr;
        config.imageCallback = &ImguiMarkdownRender::imageCallback;
        config.headingFormats[0] = {headerFonts[0], true};
        config.headingFormats[1] = {headerFonts[1], true};
        config.headingFormats[2] = {headerFonts[2], false};
        config.userData = this;
    }

    void ImguiMarkdownRender::render(const ST::string& markdown) {
        Markdown(markdown.c_str(), markdown.size() - 1, config);
    }
}
