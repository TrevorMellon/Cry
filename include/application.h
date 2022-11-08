#ifndef APPLICATION_H
#define APPLICATION_H

#include <cry/cry.h>

#include <memory>

namespace cry {

    class Application
    {
        public:
            Application();
            ~Application() = default;

            // ========================================

            void parse_options ( int argc, char **argv );

        private:
            std::unique_ptr<Cry>     _cry;
    };

} // namespace cry

#endif //APPLICATION_H
