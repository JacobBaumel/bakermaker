#ifndef BAKERMAKER_CLIENTEXTRACT_H
#define BAKERMAKER_CLIENTEXTRACT_H

#include "UI/BaseUIScreen.h"
#include "string_theory/string"
#include <atomic>
#include <thread>

namespace bakermaker {
    class ClientExtract : public BaseUIScreen {
    private:
        std::thread* exec;
        std::atomic_bool execDone;
        std::atomic_int success;

        void extractClient(const ST::string ip, const int port);

    public:
        ClientExtract();
        void render() override;
    };
}

#endif //BAKERMAKER_CLIENTEXTRACT_H
