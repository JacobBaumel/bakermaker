//
// Created by skipp on 5/1/2024.
//

#ifndef BAKERMAKER_CREATEADMINKEY_H
#define BAKERMAKER_CREATEADMINKEY_H

#include "UI/BaseUIScreen.h"
#include "string_theory/string"
#include <vector>
#include <thread>
#include <atomic>
#include "utils.h"

namespace bakermaker {
    class CreateAdminKey : public BaseUIScreen {
    private:
        ST::string instructions;

        char newName[USERLENGTH] = {'\0'};

        std::thread* exec;
        std::atomic_bool execDone;
        std::atomic_int success;


    public:
        CreateAdminKey();
        void render() override;
        
    };
}

#endif //BAKERMAKER_CREATEADMINKEY_H
