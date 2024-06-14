#include "UI/LibsNotFound.h"
#include "UI/BaseUIScreen.h"
#include <thread>
#include "romfs/romfs.hpp"
#include "improgress.h"
#include <fstream>
#include <iostream>
#include "unzip_utils.h"
#include "utils.h"

namespace bakermaker {
    LibsNotFound::LibsNotFound() : BaseUIScreen(bakermaker::ProgramStage::LIBS_NOT_FOUND, nullptr),
        extrDone(false) {
        extrThread = new std::thread([this](){
            romfs::Resource libs = romfs::get("libs.zip");
            unzip_from_mem((void*) libs.data(), libs.size());

            extrDone = true;
        });
    }

    void LibsNotFound::render() {
        ImGui::SetNextWindowFocus();
        ImGui::SetNextWindowSize(MODAL_SIZE);
        ImVec2 screenSize = ImGui::GetIO().DisplaySize;
        screenSize.x = (screenSize.x - MODAL_SIZE.x) / 2;
        screenSize.y = (screenSize.y - MODAL_SIZE.y) / 2;
        ImGui::SetNextWindowPos(screenSize);
        if(ImGui::Begin("Test Modal", nullptr, modalFlags)) {
            ImGui::Text("Extracting Libraries");
            if(!extrDone) {
                ImGui::SameLine();
                bakermaker::spinner();
            }

            else ImGui::Text("Library Extraction Complete. Please restart the program.");
        }

        ImGui::End();
    }
}