#ifndef APPLICATION_H
#define APPLICATION_H

namespace cry {

    class Application {
    public:
        Application();
        ~Application();
    public:
        void parse_options(int argc, char** argv);
    };

} // namespace cry

#endif //APPLICATION_H
