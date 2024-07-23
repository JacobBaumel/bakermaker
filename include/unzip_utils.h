#ifndef BAKERMAKER_UNZIP_UTILS_H
#define BAKERMAKER_UNZIP_UTILS_H

namespace bakermaker {
    // Unzip a zip archive in memory to the working directory. Made with libromfs in mind
    bool unzip_from_mem(const void* data, size_t size);

    // Zip the following files into a "debug package": keyfile, config.json, gitolite.conf, install.log
    // and the contents of the "keys/" directory
    bool zip_debug();
}
#endif //BAKERMAKER_UNZIP_UTILS_H
