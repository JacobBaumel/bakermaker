//
// Created by skipp on 4/17/2024.
//

#ifndef BAKERMAKERSETUPER_IMPROGRESS_H
#define BAKERMAKERSETUPER_IMPROGRESS_H

#include "imgui.h"

namespace ImGui {
    bool BufferingBar(const char *label, float value, const ImVec2 &size_arg,
                      const ImU32 &bg_col, const ImU32 &fg_col);

    bool Spinner(const char *label, float radius, int thickness, const ImU32 &color);
}
#endif //BAKERMAKERSETUPER_IMPROGRESS_H
