#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "GLFW/glfw3.h"
#include "romfs/romfs.hpp"
#include "stbi_funcs.h"

namespace bakermaker {
    bool loadTextureFromRomfs(const char* filename, GLuint* outtex, int* outwidth, int* outheight) {
        int image_width = 0;
        int image_height = 0;
        romfs::Resource image = romfs::get(filename);
        unsigned char* image_data = stbi_load_from_memory((unsigned char*) image.data(), image.size(),
                                                          &image_width, &image_height, nullptr, 4);
        if (image_data == NULL)
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

        // Upload pixels into texture
#if defined(GL_UNPACK_ROW_LENGTH) && !defined(__EMSCRIPTEN__)
        glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
#endif
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height,
                     0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
        stbi_image_free(image_data);

        *outtex = image_texture;
        *outwidth = image_width;
        *outheight = image_height;

        return true;
    }
}