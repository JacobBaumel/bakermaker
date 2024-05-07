//
// Created by skipp on 5/1/2024.
//

#ifndef BAKERMAKER_CREATESSHKEYS_H
#define BAKERMAKER_CREATESSHKEYS_H

#include "UI/BaseUIScreen.h"
#include "string_theory/string"
#include <map>
#include <thread>
#include <atomic>

namespace bakermaker {
    class CreateSSHKeys : public BaseUIScreen {
    private:
        static constexpr int USERLENGTH = 65;
        ST::string instructions;
        char adminname[USERLENGTH]{'\0'};
        std::map<char*, bool*> users;
        bool hasConfirmed;

        std::thread* exec;
        std::atomic_bool execDone;
        std::atomic_int execProgress;
        std::atomic_size_t execTotal;


    public:
        CreateSSHKeys();
        void render() override;
        
    };
}

#endif //BAKERMAKER_CREATESSHKEYS_H
