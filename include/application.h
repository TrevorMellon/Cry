#ifndef APPLICATION_H
#define APPLICATION_H

#include <cry/cry.h>

namespace cry {

    class Application {
    public:
        Application();
        ~Application();
    public:
        void parse_options(int argc, char** argv);        

    private:       
        Cry     *_cry;
    };

} // namespace cry

#endif //APPLICATION_H
