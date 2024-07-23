#include <filesystem>

#include "miniz.h"
#include "string_theory/string"

#include "unzip_utils.h"

namespace bakermaker {
    bool unzip_from_mem(const void* data, const size_t size) {
        mz_zip_archive arch{};
        mz_zip_reader_init_mem(&arch, data, size, 0);
        const mz_uint totalFiles = mz_zip_reader_get_num_files(&arch);

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
        static constexpr auto files = {"keyfile", "config.json", "gitolite.conf", "install.log"};
        mz_zip_archive arch{};
        bool success = true;
        success = mz_zip_writer_init_file(&arch, "debug.zip", 0);

        for(const auto& file : files) {
            if(std::filesystem::exists(file)) success = success && mz_zip_writer_add_file(
                &arch, file, file, nullptr, 0, MZ_BEST_COMPRESSION);
        }

        // For some reason this function "mz_zip_writer_add_file" also writes to disk. There is no dedicated
        // "write to disk" function. It gets the target filename from the "mz_zip_writer_init_file" function above
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
