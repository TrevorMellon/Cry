
#include <application.h>

using namespace cry;

int main(int argc, char **argv) {
  Application app(argc, argv);
  app.ParseOptions();
  return 0;
}
