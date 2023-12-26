#include <application.h>
#include <boost/filesystem.hpp>
#include <cry/common.h>
#include <cry/cry.h>
#include <sstream>
#include <vector>

#include <gtest/gtest.h>

namespace fs = boost::filesystem;

TEST(APP, ENCDEC) {
  using namespace cry;
  const char *file = TEST_FILE;

  std::vector<std::string> tmp = {"app_test", "-e", file};
  std::vector<char *> tmp2;
  std::for_each(tmp.begin(), tmp.end(), [&tmp2](const std::string &str) {
    tmp2.push_back(const_cast<char *>(str.c_str()));
  });
  char **pargv = tmp2.data();
  Application app(tmp.size(), pargv);
  app.ParseOptions();

  fs::path p(file);
  if (fs::exists(p)) {
    fs::remove(p);
  }

  std::stringstream ss;
  ss << file << ".cry";
  tmp.clear();
  tmp = {"app_test", "-d", ss.str()};
  tmp2.clear();
  std::for_each(tmp.begin(), tmp.end(), [&tmp2](const std::string &str) {
    tmp2.push_back(const_cast<char *>(str.c_str()));
  });
  pargv = tmp2.data();
  Application app2(tmp.size(), pargv);
  app2.ParseOptions();
}
