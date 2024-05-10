//
// Created by skipp on 5/10/2024.
//

#ifndef BAKERMAKER_UTILS_H
#define BAKERMAKER_UTILS_H

#include "ImguiMarkdownRender.h"
#include "UI/BaseUIScreen.h"
#include "json11.h"
#include "string_theory/string"

namespace ST {
    string operator+(const string& str, int num);
}

namespace bakermaker {
    extern ImguiMarkdownRender* documentation;
    extern bakermaker::ProgramStage stage;
    extern ST::string documarkdown;
    extern Json config;
}

#endif //BAKERMAKER_UTILS_H
