
#ifndef CRY_H
#define CRY_H

#include <cry/config.h>
#include <cry/common.h>

#include <string>

namespace cry {

    struct CryptDetail {
        size_t          keylength;
        std::string     key;
        size_t          blocklength;
        std::string     block;
    };

    class Cry
    {
        public:
            Cry();
            ~Cry() = default;

            // ==========================================

            void encrypt ( std::string file, EncryptionType type = gpg_algo );
            void decrypt ( std::string file, EncryptionType type = gpg_algo );
            void setOutput ( std::string output );
            void setPassword ( std::string password );

            // ==========================================

        private:

            CryptDetail encryptionSetup (
                gcry_cipher_hd_t &hd,
                EncryptionType type
            );

            std::vector<uint8_t>
            encryptBlock (
                const std::vector<uint8_t> &bufferin,
                const size_t blockSize,
                gcry_cipher_hd_t &hd );

            std::vector<uint8_t> padToBlock (
                const std::vector<uint8_t> &in,
                const int blocksize
            );

            void encryptImpl ( std::string file, EncryptionType type = gpg_algo );
            void decryptImpl ( std::string file, EncryptionType type = gpg_algo );

            size_t
            identifyFile ( std::string file );

            CryptDetail
            getCryptDetails ( EncryptionType type );

            std::string
            cryptToLength ( std::string in, size_t len );

            // ==========================================

            std::string _password;
            std::string _output;
    };
}

#endif // CRY_H
