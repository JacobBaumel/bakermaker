#include <filesystem>
#include <fstream>

#include "romfs/romfs.hpp"

#include "UI/ClientExtract.h"
#include "utils.h"

namespace bakermaker {
    using namespace ST::literals;

    ClientExtract::ClientExtract() : BaseUIScreen(ProgramStage::CLIENT_EXTRACT, &configScreens), exec(nullptr),
                                     execDone(false), success(0) {
    }

    void ClientExtract::render() {
        // Section header
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
            exec = new std::thread(&ClientExtract::extractClient, this, config["server"]["ip"].get<ST::string>(),
                                   config["server"]["port"].get<int>());
            ImGui::BeginDisabled();
        }

        if(exec && !execDone) {
            ImGui::EndDisabled();
            ImGui::SameLine();
            spinner();
        }

        if(execDone) {
            if(exec) {
                exec->join();
                delete exec;
                exec = nullptr;

                if(success != 0) startErrorModal("Error extracting client setup!");
                else config["extracting"] = false;
            }

            if(success == 0) {
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0, 1, 0, 1));
                ImGui::Text("Success!");
                ImGui::PopStyleColor();
            }
        }
    }

    void ClientExtract::extractClient(const ST::string ip, const int port) {
        success = 0;

        // If a client program has already been extracted, delete it and create a new directory
        if(std::filesystem::exists("bakermakerclient")) {
            std::filesystem::remove_all("bakermakerclient");
        }

        std::filesystem::create_directories("bakermakerclient");

        // Load client program from romfs and write to disk
        {
            romfs::Resource bmc = romfs::get("bakermakerclient.exe");
            FILE* file;
            fopen_s(&file, "bakermakerclient/bakermakerclient.exe", "wb");
            size_t nwritten = fwrite(bmc.data(), 1, bmc.size(), file);
            fclose(file);

            if(nwritten != bmc.size()) {
                success = 1;
                execDone = true;
            }
        }

        // Copy private keys to the client program directory
        std::filesystem::create_directories("bakermakerclient/keys");
        for(const auto& file : std::filesystem::directory_iterator("keys")) {
            if(file.path().string().ends_with(".pub")) continue;
            ST::string dest = file.path().string();
            dest = dest.after_last('\\');
            copy_file(file, ("bakermakerclient/keys/"_st + dest).c_str());
        }

        // Create the "ip" file
        std::ofstream ipfile("bakermakerclient/ip");
        ipfile << ip.c_str() << '\n';
        ipfile << port;
        ipfile.close();
        execDone = true;
    }
}
