//
// Created by skipp on 4/29/2024.
//

#ifndef TEST_MD_SSH_HELPER_H
#define TEST_MD_SSH_HELPER_H

#include <string_theory/string>
#include "libssh/libssh.h"
#include "libssh/sftp.h"

namespace bakermaker {
    int createSession(ssh_session& session, const char* ip, const char* user, const char* keyfile, int port = 22);
    int genSSHKeyToFile(const char* filename);
    int uploadToRemote(sftp_session s, const char* filepath, const char* remotepath);
}

#endif //TEST_MD_SSH_HELPER_H
