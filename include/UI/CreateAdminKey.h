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

namespace bakermaker {
    class CreateAdminKey : public BaseUIScreen {
    private:
        static constexpr int USERLENGTH = 65;
        static void createUser(const char* name, std::atomic_bool* execDone,
                               std::atomic_int* success);
        static void deleteUser(const char* name, std::atomic_bool* execDone,
                               std::atomic_int* success);

        ST::string instructions;

        char* newName;

        std::thread* exec;
        std::atomic_bool execDone;
        std::atomic_int success;


    public:
        CreateAdminKey();
        void render() override;
        
    };
}

#endif //BAKERMAKER_CREATEADMINKEY_H
