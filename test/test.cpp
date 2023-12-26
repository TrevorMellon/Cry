
#include <boost/filesystem.hpp>
#include <cry/common.h>
#include <cry/cry.h>

#include <gtest/gtest.h>

namespace fs = boost::filesystem;

TEST(Basic, ENCDEC) {
  using namespace cry;
  const char *file = TEST_FILE;
  std::cout << "Using: " << file << std::endl;
  Cry *cry = new Cry();
  std::string password = "testpassword";

  cry->SetPassword(password);
  cry->Encrypt(file);
  std::stringstream cryfile;
  cryfile << file << ".cry";
  fs::path p(file);
  fs::remove(p);
  cry->Decrypt(cryfile.str());
  delete cry;
}

TEST(Header, io) {
  using namespace cry;
  char buffer[1024];
  memset(buffer, 0, 1024);
  std::vector<uint8_t> vec;
  vec.reserve(1024);
  vec.assign(buffer, buffer + 1023);
  CryHeader hdrin;
  hdrin.SetFileSize(512);
  std::string test = "test.txt";
  hdrin.SetFilename(test);
  auto h = hdrin.WriteHeader();
  CryHeader hdrout;
  hdrout.ReadHeader(h);
  std::string readtest = std::string(hdrout.Filename(), hdrout.FilenameSize());
  ASSERT_EQ(test, readtest);
  size_t fsz = hdrout.FileSize();
  ASSERT_EQ(512, fsz);
}
