#include <filesystem>
#include "miniz.h"
#include "unzip_utils.h"

namespace bakermaker {
    bool unzip_from_mem(void* data, size_t size) {
        mz_zip_archive arch;
        memset((void*) &arch, 0, sizeof(arch));
        mz_zip_reader_init_mem(&arch, data, size, 0);
        mz_uint totalFiles = mz_zip_reader_get_num_files(&arch);

        for (mz_uint i = 0; i < totalFiles; i++) {
            char archname[128];
            mz_zip_reader_get_filename(&arch, i, archname, 128);

            if(mz_zip_reader_is_file_a_directory(&arch, i))
                std::filesystem::create_directories(std::string(archname));

            else
                mz_zip_reader_extract_file_to_file(&arch, archname, archname, 0);
        }

        mz_free(&arch);

        return true;
    }
}