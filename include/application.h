#ifndef APPLICATION_H
#define APPLICATION_H

#include <string>
#include <config.h>

namespace cry {

    typedef gcry_cipher_algos EncryptionType;

    struct CryHeader
    {
        uint64_t    filesize;
        uint64_t    filenamesize;
        char*       filename;

        CryHeader()
        {
            filename = nullptr;
            filenamesize = 0;
            filesize = 0;
        }

        ~CryHeader()
        {
            if(filename)
            {
                delete[] filename;
                filename = nullptr;
            }
        }

    };

    struct CryptDetail
    {
        size_t          keylength;
        std::string     key;
        size_t          blocklength;
        std::string     block;
    };

    class Application {
    public:
        Application();
        ~Application();
    public:
        void parse_options(int argc, char** argv);
        void encrypt(std::string file, EncryptionType type=GPG_ALGO);
        void decrypt(std::string file, EncryptionType type=GPG_ALGO);
        size_t identifyFile(std::string file);
        CryptDetail getCryptDetails(EncryptionType type=GPG_ALGO);
    public:
        size_t writeHeader(unsigned char* buffer, const CryHeader &hdr);
        size_t readHeader(const unsigned char* buffer, CryHeader *hdr);
    protected:
        void encryptImpl(std::string file, EncryptionType type=GPG_ALGO);
        void decryptImpl(std::string file, EncryptionType type=GPG_ALGO);
    private:
        std::string cryptToLength(std::string in, size_t len);
    private:       
        std::string     _password;
    };

} // namespace cry

#endif //APPLICATION_H
