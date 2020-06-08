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
    private:
        std::string cryptToLength(std::string in, size_t len);
    };

} // namespace cry

#endif //APPLICATION_H
