//
// Created by skipp on 5/10/2024.
//

#ifndef BAKERMAKER_UTILS_H
#define BAKERMAKER_UTILS_H

#include "ImguiMarkdownRender.h"
#include "UI/BaseUIScreen.h"
#include "nlohmann/json.hpp"
#include "string_theory/string"

namespace bakermaker {
    extern ImguiMarkdownRender* documentation;
    extern bakermaker::ProgramStage stage;
    extern ST::string documarkdown;
    extern nlohmann::json config;


    void startErrorModal(const char* error);
    void displayErrorModal();
    void spinner();
}

#endif //BAKERMAKER_UTILS_H
