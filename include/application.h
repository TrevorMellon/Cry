#ifndef APPLICATION_H
#define APPLICATION_H

#include <string>
#include <config.h>

namespace cry {

    typedef gcry_cipher_algos EncryptionType;

    class Application {
    public:
        Application();
        ~Application();
    public:
        void parse_options(int argc, char** argv);
        void encrypt(std::string file, EncryptionType type=GPG_ALGO);
        void decrypt(std::string file, EncryptionType type=GPG_ALGO);
        void identifyFile(std::string file);
    protected:
        void encryptImpl(std::string file, EncryptionType type=GPG_ALGO);
        void decryptImpl(std::string file, EncryptionType type=GPG_ALGO);
    private:
        std::string cryptToLength(std::string in, size_t len);
    private:
        std::string     _fileName;
        uint64_t        _fileSize;
        bool            _encrypted;
        std::string     _originalFileName;
        std::string     _password;
    };

} // namespace cry

#endif //APPLICATION_H
