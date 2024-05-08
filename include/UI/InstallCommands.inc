//
// Created by skipp on 5/8/2024.
//

#ifndef BAKERMAKER_INSTALLCOMMANDS_INC
#define BAKERMAKER_INSTALLCOMMANDS_INC

namespace bakermaker {
    static constexpr int COMMANDS_LENGTH = 2;
    static constexpr char commands[COMMANDS_LENGTH][32] = {
            "sudo apt update",
            "sudo apt upgrade -y"};
}

#endif //BAKERMAKER_INSTALLCOMMANDS_INC