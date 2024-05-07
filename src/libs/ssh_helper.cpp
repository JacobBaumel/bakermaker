#include "ssh_helper.h"
#include <iostream>

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

            std::cout << "here1" << std::endl;
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

        rc = ssh_pki_export_pubkey_file(pub, ((ST::string(file) + ".p")).c_str());
        if(rc != SSH_OK) return -4;

        return 0;
    }
}