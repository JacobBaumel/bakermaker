//
// Created by skipp on 5/8/2024.
//

#ifndef BAKERMAKER_INSTALLCOMMANDS_INC
#define BAKERMAKER_INSTALLCOMMANDS_INC

namespace bakermaker {
    static constexpr int COMMANDS_LENGTH = 10;
    static constexpr char commands[COMMANDS_LENGTH][64] = {
            "sudo apt update",
            "sudo apt upgrade -y",
            "sudo apt install git -y",
            "sudo adduser --gecos \"\" --disabled-password git"};
}

#endif //BAKERMAKER_INSTALLCOMMANDS_INC