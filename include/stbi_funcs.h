//
// Created by skipp on 4/27/2024.
//

#ifndef BAKERMAKER_STBI_FUNCS_H
#define BAKERMAKER_STBI_FUNCS_H

#include "GLFW/glfw3.h"

namespace bakermaker {
    bool loadTextureFromRomfs(const char* filename, GLuint* outtex, int* outwidth, int* outheight);
}

#endif //BAKERMAKER_STBI_FUNCS_H
