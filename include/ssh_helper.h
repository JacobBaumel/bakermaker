//
// Created by skipp on 4/29/2024.
//

#ifndef TEST_MD_SSH_HELPER_H
#define TEST_MD_SSH_HELPER_H

#include <string_theory/string>
#include "libssh/libssh.h"

namespace bakermaker {
    int createSession(ssh_session& session, const char* ip, const char* user, const char* keyfile, int port = 22);
    int genSSHKeyToFile(const char* filename);
}

#endif //TEST_MD_SSH_HELPER_H
