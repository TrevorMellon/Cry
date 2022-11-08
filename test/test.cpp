
#include <cry/common.h>
#include <cry/cry.h>

#include <gtest/gtest.h>

TEST ( Basic, ENCDEC )
{
    using namespace cry;
    const char *file = TEST_FILE;
    Cry *cry = new Cry();
    cry->encrypt ( file );
    std::stringstream cryfile;
    cryfile << file << ".cry";
    cry->decrypt ( cryfile.str() );
    delete cry;
}

TEST ( Header, io )
{
    using namespace cry;
    char buffer[1024];
    memset ( buffer, 0, 1024 );
    std::vector<uint8_t> vec;
    vec.reserve ( 1024 );
    vec.assign ( buffer, buffer + 1023 );
    CryHeader hdrin;
    hdrin.setFileSize ( 512 );
    std::string test = "test.txt";
    hdrin.setFilename ( test );
    auto h = hdrin.writeHeader ();
    CryHeader hdrout;
    hdrout.readHeader ( h );
    std::string readtest = std::string ( hdrout.filename(), hdrout.filenameSize() );
    ASSERT_EQ ( test, readtest );
    size_t fsz = hdrout.fileSize();
    ASSERT_EQ ( 512, fsz );
}
