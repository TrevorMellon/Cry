#ifndef APPLICATION_H
#define APPLICATION_H

#include <string>

namespace cry {

    class Application {
    public:
        Application();
        ~Application();
    public:
        void parse_options(int argc, char** argv);
        void encrypt(std::string file);
        void decrypt(std::string file);
        void identifyFile(std::string file);
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
