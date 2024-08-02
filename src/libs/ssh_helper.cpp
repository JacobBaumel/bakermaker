#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <sys/stat.h>

#include "libssh/libssh.h"
#include "libssh/sftp.h"
#include "string_theory/string"

#include "ssh_helper.h"


namespace bakermaker {
    using namespace ST::literals;

    int createSession(ssh_session& session, const char* ip, const char* user, const char* keyfile, int port) {
        // Check if any arguments are invalid
        if((ip == nullptr || ip[0] == '\0') || (user == nullptr || user[0] == '\0') ||
            (keyfile == nullptr || keyfile[0] == '\0')) {
            return -1;
        }

        // Create session object and set options
        session = ssh_new();
        ssh_options_set(session, SSH_OPTIONS_HOST, ip);
        ssh_options_set(session, SSH_OPTIONS_LOG_VERBOSITY, SSH_LOG_NONE);
        ssh_options_set(session, SSH_OPTIONS_PORT, &port);
        ssh_options_set(session, SSH_OPTIONS_USER, user);

        // Test connectivity with server
        int rc = ssh_connect(session);
        if(rc != SSH_OK) {
            ssh_free(session);
            return -2;
        }

        // Read in private key file, and convert to public key as well
        ssh_key prkey, pubkey;
        if(SSH_OK != ssh_pki_import_privkey_file(keyfile, "", nullptr,
                                                 nullptr, &prkey)) {
            ssh_free(session);
            std::cout << "Error reading private key" << std::endl;
            return -5;
        }
        ssh_pki_export_privkey_to_pubkey(prkey, &pubkey);

        // Test public key
        rc = ssh_userauth_try_publickey(session, nullptr, pubkey);
        if(rc != SSH_AUTH_SUCCESS) {
            ssh_key_free(prkey);
            ssh_key_free(pubkey);
            ssh_free(session);
            return -3;
        }

        // Auth with private key
        rc = ssh_userauth_publickey(session, nullptr, prkey);
        if(rc != SSH_AUTH_SUCCESS) {
            ssh_key_free(prkey);
            ssh_key_free(pubkey);
            ssh_free(session);
            return -4;
        }

        ssh_key_free(prkey);
        ssh_key_free(pubkey);

        return 0;
    }

    int genSSHKeyToFile(const char* file) {
        // Generate 4096 bit ED25519 key
        ssh_key key = nullptr;
        int rc = ssh_pki_generate(SSH_KEYTYPE_ED25519, 4096, &key);
        if(rc != SSH_OK) return -1;

        // Export private key to file
        rc = ssh_pki_export_privkey_file(key, nullptr, nullptr, nullptr, file);
        if(rc != SSH_OK) {
            ssh_key_free(key);
            return -2;
        }

        // Extract public key
        ssh_key pub = nullptr;
        rc = ssh_pki_export_privkey_to_pubkey(key, &pub);
        if(rc != SSH_OK) {
            ssh_key_free(key);
            return -3;
        }

        // Write public key to file
        rc = ssh_pki_export_pubkey_file(pub, (ST::string(file) + ".pub").c_str());
        ssh_key_free(key);
        ssh_key_free(pub);

        return rc == SSH_OK ? 0 : -4;
    }

#define FILE_FLAGS O_WRONLY | O_CREAT | O_TRUNC, S_IREAD | S_IWRITE | S_IEXEC

    int uploadToRemote(sftp_session s, const char* filepath, const char* remotepath) {
        // Open the remote file
        sftp_file file = sftp_open(s, remotepath, FILE_FLAGS);

        if(file == nullptr) return -1;

        // Open the local file to read from
        FILE* localfile;
        fopen_s(&localfile, filepath, "r");

        if(localfile == nullptr) return -2;

        // Read in bytes from the local file, and write to remote file
        char buffer[256];
        size_t nbytes = fread_s(buffer, 256, 1, 256, localfile);
        while(nbytes > 0) {
            if(sftp_write(file, buffer, nbytes) == 0) {
                sftp_close(file);
                return -4;
            }

            nbytes = fread_s(buffer, 256, 1, 256, localfile);
        }

        int rc = sftp_close(file);
        fclose(localfile);
        if(rc != SSH_OK) return -3;

        return 0;
    }

    int uploadToRemote(sftp_session s, void* data, size_t size, const char* remotepath) {
        sftp_file file = sftp_open(s, remotepath, FILE_FLAGS);

        if(file == nullptr) return -1;
        size_t rc = sftp_write(file, data, size);

        return rc == 0 ? -2 : 0;
    }

    int downloadToLocal(sftp_session sftp, const char* remotepath, const char* localpath, const size_t block_size) {
        // Open remote and local files
        sftp_file file = sftp_open(sftp, remotepath, O_RDONLY, 0);

        if(file == nullptr) return -1;

        FILE* localfile = fopen(localpath, "wb");
        if(localfile == nullptr) {
            sftp_close(file);
            return -2;
        }

        // Read bytes from server, and write to file
        while(true) {
            char* buf = new char[block_size];
            ssize_t nbytes = sftp_read(file, buf, block_size);
            if(nbytes == 0) break;
            if(nbytes < 0) {
                fclose(localfile);
                sftp_close(file);
                delete[] buf;
                return -3;
            }


            size_t nwritten = fwrite(buf, 1, nbytes, localfile);

            if(nwritten != nbytes) {
                fclose(localfile);
                sftp_close(file);
                delete[] buf;
                return -4;
            }

            delete[] buf;
        }

        fclose(localfile);
        sftp_close(file);
        return 0;
    }

    int runCommand(ssh_session session, const char* command) {
        ssh_channel ch = ssh_channel_new(session);
        if(ch == nullptr) return -1;
        ssh_channel_open_session(ch);
        ssh_channel_request_exec(ch, command);

        int rc = ssh_channel_get_exit_status(ch);
        ssh_channel_send_eof(ch);
        ssh_channel_close(ch);
        ssh_channel_free(ch);

        return rc;
    }
}
