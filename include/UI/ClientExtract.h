#ifndef BAKERMAKER_CLIENTEXTRACT_H
#define BAKERMAKER_CLIENTEXTRACT_H

#include <atomic>
#include <thread>

#include "string_theory/string"
#include "UI/BaseUIScreen.h"

namespace bakermaker {
    // UI Element which allows extracting the client program and preparing its environment for distribution
    class ClientExtract : public BaseUIScreen {
        std::thread* exec;
        std::atomic_bool execDone;
        std::atomic_int success;

        // To be called from a thread
        void extractClient(ST::string ip, int port);

    public:
        ClientExtract();
        void render() override;
    };
}

#endif //BAKERMAKER_CLIENTEXTRACT_H
