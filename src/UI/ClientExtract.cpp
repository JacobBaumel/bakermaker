#include "UI/ClientExtract.h"
#include <filesystem>
#include "romfs/romfs.hpp"
#include "utils.h"
#include <fstream>
#include <chrono>

namespace bakermaker {
    ClientExtract::ClientExtract()
            :
            BaseUIScreen(bakermaker::ProgramStage::CLIENT_EXTRACT, &bakermaker::configScreens),
            exec(nullptr), execDone(false), success(0) {}

    void ClientExtract::render() {
        ImGui::PushFont(fontlist[1]);
        ImGui::Text("Extract Client");
        ImGui::PopFont();
        ImGui::Separator();

        ImGui::Text("Extract program to set up clients based on added users");

        if(exec && !execDone) ImGui::BeginDisabled();

        if(ImGui::Button("Extract")) {
            config["extracting"] = true;
            execDone = false;
            success = 0;
            exec = new std::thread(&ClientExtract::extractClient, this, config["server"]["ip"].get<ST::string>());
            ImGui::BeginDisabled();
        }

        if(exec && !execDone) {
            ImGui::EndDisabled();
            ImGui::SameLine();
            bakermaker::spinner();
        }

        if(execDone) {
            if(exec) {
                exec->join();
                delete exec;
                exec = nullptr;

                if(success != 0) bakermaker::startErrorModal("Error extracting client setup!");
                else config["extracting"] = false;
            }

            if(success == 0) {
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0, 1, 0, 1));
                ImGui::Text("Success!");
                ImGui::PopStyleColor();
            }
        }
    }

    void ClientExtract::extractClient(const ST::string ip) {
        using namespace ST::literals;

        success = 0;
        std::filesystem::create_directories("bakermakerclient");

        {
            romfs::Resource bmc = romfs::get("bakermakerclient.exe");
            FILE* file;
            fopen_s(&file, "bakermakerclient/bakermakerclient.exe", "wb");
            size_t nwritten = fwrite((void*) bmc.data(), 1, bmc.size(), file);
            fclose(file);

            if(nwritten != bmc.size()) {
                success = 1;
                execDone = true;
            }
        }

        std::filesystem::create_directories("bakermakerclient/keys");
        for(const auto& file : std::filesystem::directory_iterator("keys")) {
            if(file.path().string().ends_with(".pub")) continue;
            ST::string dest = file.path().string();
            dest = dest.after_last('\\');
            std::filesystem::copy_file(file, ("bakermakerclient/keys/"_st + dest).c_str());
        }

        std::ofstream ipfile("bakermakerclient/ip");
        ipfile << ip.c_str();
        ipfile.close();

        using namespace std::literals;
        std::this_thread::sleep_for(2000ms);

        execDone = true;
    }
}