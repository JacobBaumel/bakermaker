#include "libssh/libssh.h"
#include "libssh/sftp.h"
#include "ssh_helper.h"
#include <iostream>
#include <fcntl.h>
#include <sys/stat.h>
#include <fstream>

namespace bakermaker {
    int createSession(ssh_session& session, const char* ip, const char* user, const char* keyfile, int port) {
        session = ssh_new();

        if(session == nullptr || (ip == nullptr || ip[0] == '\0') || (user == nullptr || user[0] == '\0') ||
                (keyfile == nullptr || keyfile[0] == '\0')) {
            return -1;
        }

        ssh_options_set(session, SSH_OPTIONS_HOST, ip);
        ssh_options_set(session, SSH_OPTIONS_LOG_VERBOSITY, SSH_LOG_NONE);
        ssh_options_set(session, SSH_OPTIONS_PORT, &port);
        ssh_options_set(session, SSH_OPTIONS_USER, user);

        int rc = ssh_connect(session);
        if(rc != SSH_OK) {
            std::cout << "Error connecting to SSH server!" << '\n';
            std::cout << ssh_get_error(session) << std::endl;
            ssh_free(session);
            return -2;
        }

        ssh_key prkey, pubkey;
        if(SSH_OK != ssh_pki_import_privkey_file(keyfile, "", nullptr,
                                                 nullptr,&prkey)) {
            ssh_free(session);
            std::cout << "Error reading private key" << std::endl;
            return -5;
        };
        ssh_pki_export_privkey_to_pubkey(prkey, &pubkey);

        rc = ssh_userauth_try_publickey(session, nullptr, pubkey);

        if(rc != SSH_AUTH_SUCCESS) {
            ssh_key_free(prkey);
            ssh_key_free(pubkey);
            ssh_free(session);
            if(rc == SSH_AUTH_DENIED)
                std::cout << "Public key not accepted" << std::endl;
            else std::cout << "Could not connect to ssh server" << std::endl;

            return -3;
        }

        rc = ssh_userauth_publickey(session, nullptr, prkey);
        if(rc != SSH_AUTH_SUCCESS) {
            ssh_key_free(prkey);
            ssh_key_free(pubkey);
            ssh_free(session);
            if(rc == SSH_AUTH_DENIED)
                std::cout << "Private key not accepted" << std::endl;
            else std::cout << "Could not connect to ssh server" << std::endl;

            return -4;
        }

        ssh_key_free(prkey);
        ssh_key_free(pubkey);

        return 0;
    }

    int genSSHKeyToFile(const char* file) {
        ssh_key key;
        memset((void*) &key, 0, sizeof(ssh_key));
        int rc = ssh_pki_generate(ssh_keytypes_e::SSH_KEYTYPE_ED25519, 4096, &key);
        if(rc != SSH_OK) return -1;

        rc = ssh_pki_export_privkey_file(key, nullptr, nullptr, nullptr, file);
        if(rc != SSH_OK) return -2;

        ssh_key pub;
        memset((void*) &pub, 0, sizeof(ssh_key));
        rc = ssh_pki_export_privkey_to_pubkey(key, &pub);
        if(rc != SSH_OK) return -3;

        rc = ssh_pki_export_pubkey_file(pub, ((ST::string(file) + ".pub")).c_str());
        if(rc != SSH_OK) return -4;

        return 0;
    }

#define FILE_FLAGS O_WRONLY | O_CREAT | O_TRUNC, S_IREAD | S_IWRITE | S_IEXEC

    int uploadToRemote(sftp_session s, const char* filepath, const char* remotepath) {
        sftp_file file = sftp_open(s, remotepath, FILE_FLAGS);

        if(file == nullptr) return -1;

        FILE* localfile;
        fopen_s(&localfile, filepath, "r");

        if(localfile == nullptr) return -2;

        char buffer[256];
        size_t nbytes = fread_s(buffer, 256, 1, 256, localfile);
        while(nbytes > 0) {
            if(sftp_write(file, buffer, nbytes) == 0) {
                sftp_close(file);
                return -4;
            }

            nbytes = fread_s(buffer, 256, 1, 256, localfile);
        }

        {
            int rc = sftp_close(file);
            if(rc != SSH_OK) return -3;
        }

        return 0;
    }

    int uploadToRemote(sftp_session s, void* data, size_t size, const char* remotepath) {
        sftp_file file = sftp_open(s, remotepath, FILE_FLAGS);

        if(file == nullptr) return -1;
        size_t rc = sftp_write(file, data, size);

        return rc == 0 ? -2 : 0;
    }

    int downloadToLocal(sftp_session sftp, const char* remotepath, const char* localpath, const size_t block_size) {
        sftp_file file = sftp_open(sftp, remotepath, O_RDONLY, 0);

        if(file == nullptr) return -1;

        std::ofstream localfile(localpath, std::ofstream::binary);

        while(true) {
            char* buf = new char[block_size];
            ssize_t nbytes = sftp_read(file, buf, block_size);
            if(nbytes == 0) break;
            else if(nbytes < 0) {
                localfile.close();
                sftp_close(file);
                delete[] buf;
                return -3;
            }

            localfile.write(buf, nbytes);
//
//            size_t nwritten = fwrite(buf, 1, nbytes, localfile);
//
//            if(nwritten != nbytes) {
//                localfile.close();
//                sftp_close(file);
//                delete[] buf;
//                return -4;
//            }

            delete[] buf;
        }

        localfile.close();
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