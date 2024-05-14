//
// Created by skipp on 5/1/2024.
//

#ifndef BAKERMAKER_CREATESSHKEYS_H
#define BAKERMAKER_CREATESSHKEYS_H

#include "UI/BaseUIScreen.h"
#include "string_theory/string"
#include <vector>
#include <thread>
#include <atomic>

namespace bakermaker {
    class CreateSSHKeys : public BaseUIScreen {
    private:
        static constexpr int USERLENGTH = 65;
        static void createUser(const char* name, std::atomic_bool* execDone,
                               std::atomic_int* success, std::vector<char*>* users);
        static void deleteUser(const char* name, std::atomic_bool* execDone,
                               std::atomic_int* success, std::vector<char*>* users);

        ST::string instructions;
        std::vector<char*> users;

        char* newName;

        std::thread* exec;
        std::atomic_bool execDone;
        std::atomic_int success;


    public:
        CreateSSHKeys();
        void render() override;
        
    };
}

#endif //BAKERMAKER_CREATESSHKEYS_H
