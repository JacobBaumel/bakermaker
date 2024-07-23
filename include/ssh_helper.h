#ifndef TEST_MD_SSH_HELPER_H
#define TEST_MD_SSH_HELPER_H

#include "libssh/libssh.h"
#include "libssh/sftp.h"

namespace bakermaker {
    // Function to perform all the session creation steps
    int createSession(ssh_session& session, const char* ip, const char* user, const char* keyfile, int port = 22);

    // Create a new ssh key and write to file
    int genSSHKeyToFile(const char* filename);

    // Upload file to a remote server with sftp
    int uploadToRemote(sftp_session s, const char* filepath, const char* remotepath);

    // Upload data in memory to file on remote server. Designed with libromfs in mind
    int uploadToRemote(sftp_session s, void* data, size_t size, const char* remotepath);

    // Download file from remote server and write to local file
    int downloadToLocal(sftp_session sftp, const char* remotepath, const char* localpath, size_t block_size = 128);

    // Run command without any return text
    int runCommand(ssh_session session, const char* command);
}

#endif //TEST_MD_SSH_HELPER_H
