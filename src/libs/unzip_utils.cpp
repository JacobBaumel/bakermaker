#include <filesystem>
#include "miniz.h"
#include "unzip_utils.h"
#include "string_theory/string"
#include <iostream>

namespace bakermaker {
    bool unzip_from_mem(void* data, size_t size) {
        mz_zip_archive arch;
        memset((void*)&arch, 0, sizeof(arch));
        mz_zip_reader_init_mem(&arch, data, size, 0);
        mz_uint totalFiles = mz_zip_reader_get_num_files(&arch);

        for(mz_uint i = 0; i < totalFiles; i++) {
            char archname[128];
            mz_zip_reader_get_filename(&arch, i, archname, 128);

            if(mz_zip_reader_is_file_a_directory(&arch, i))
                std::filesystem::create_directories(std::string(archname));

            else
                mz_zip_reader_extract_file_to_file(&arch, archname, archname, 0);
        }

        mz_zip_reader_end(&arch);

        return true;
    }

    bool zip_debug() {
        mz_zip_archive arch;
        memset((void*)&arch, 0, sizeof(mz_zip_archive));
        bool success = true;
        success = mz_zip_writer_init_file(&arch, "debug.zip", 0);
        if(std::filesystem::exists("keyfile")) success = success && mz_zip_writer_add_file(
            &arch, "keyfile", "keyfile", nullptr, 0, MZ_BEST_COMPRESSION);
        if(std::filesystem::exists("config.json")) success = success && mz_zip_writer_add_file(
            &arch, "config.json", "config.json", nullptr, 0, MZ_BEST_COMPRESSION);
        if(std::filesystem::exists("gitolite.conf")) success = success && mz_zip_writer_add_file(
            &arch, "gitolite.conf", "gitolite.conf", nullptr, 0, MZ_BEST_COMPRESSION);
        if(std::filesystem::exists("install.log")) success = success && mz_zip_writer_add_file(
            &arch, "install.log", "install.log", nullptr, 0, MZ_BEST_COMPRESSION);
        mz_zip_writer_add_file(&arch, "keys/", nullptr, nullptr, 0, MZ_BEST_COMPRESSION);
        for(const auto& file : std::filesystem::directory_iterator("keys")) {
            if(file.is_directory()) continue;
            mz_zip_writer_add_file(&arch, file.path().string().c_str(), file.path().string().c_str(), nullptr, 0,
                                   MZ_BEST_COMPRESSION);
        }
        success = success && mz_zip_writer_finalize_archive(&arch);
        success = success && mz_zip_writer_end(&arch);
        return success;
    }
}
