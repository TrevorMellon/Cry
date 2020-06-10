
#include <application.h>

#include <iostream>
#include <string>
#include <vector>
#include <fstream>

#include <gcrypt.h>

#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>

#define GPG_ALGO GCRY_CIPHER_SERPENT256

#define CRY_EXT .369

const char* defualt_crypt_string = "cRy-b@by-369";

namespace po = boost::program_options;
namespace fs = boost::filesystem;

using namespace cry;

Application::Application() 
{

}

Application::~Application()
{
    
}

void Application::parse_options(int argc, char **argv)
{
    po::options_description d("Allowed options");
    d.add_options()
#if CRY_ENCRYPT
            ("encrypt,e", "encrypt a file")
#endif
#if CRY_DECRYPT
            ("decrypt,d", "decrypt a file")
#endif
            ("input,f", po::value<std::string>(), "input file")
            ;
    po::positional_options_description p;
    p.add("input", -1);
    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv).options(d).positional(p).run(), vm);
    po::notify(vm);

    if(vm.count("encrypt")
            && vm.count("input"))
    {
#if CRY_ENCRYPT
        std::cout << "Encrypt \"" << vm["input"].as< std::string >() << "\"" << std::endl;
        encrypt(vm["input"].as< std::string >());
        //e
#endif
    }
    else if(vm.count("decrypt")
            && vm.count("input"))
    {
#if CRY_DECRYPT
        std::cout << "Decrypt \"" << vm["input"].as< std::string>() << "\"" << std::endl;
        //d
#endif
    }

}

void Application::encrypt(std::string file)
{
#if CRY_ENCRYPT
    gcry_cipher_hd_t hd;

    gcry_error_t err = gcry_cipher_open(&hd, GPG_ALGO, GCRY_CIPHER_MODE_CBC, 0);

    size_t keylen = gcry_cipher_get_algo_keylen(GPG_ALGO);

    std::string key = cryptToLength("ozmodus9", keylen);

    size_t blklen = gcry_cipher_get_algo_blklen(GPG_ALGO);

    std::string blk = cryptToLength("testdata##9876", blklen);

    err = gcry_cipher_setkey(hd, key.c_str(), keylen);

    err = gcry_cipher_setiv(hd, blk.c_str(), blklen);

    fs::path p(file);

    if(!fs::exists(p)
            && !fs::is_regular_file(p))
    {
        return;
    }

    std::fstream fs;
    fs.open(p.string(), std::fstream::in | std::fstream::binary);

    std::stringstream ss;
    ss << file << ".369";

    std::fstream fsout;
    fsout.open(ss.str(), std::fstream::out| std::fstream::binary);


    size_t cnt =1024 * 1024;//1Mb

    unsigned char *buffer = new unsigned char[cnt];

    auto c = fs.readsome((char*)buffer,cnt);

    unsigned char *cryptbuffer = new unsigned char[cnt];

    memset(cryptbuffer,0, cnt);

    int rem = c % blklen;
    int r = c / blklen;

    if(rem)
    {
        ++r;
    }

    size_t bblk = r * blklen;

    err = gcry_cipher_encrypt(hd, cryptbuffer, bblk, buffer, bblk);

    if(err)
    {
        auto ec = gcry_err_code(err);
        delete[] buffer;
        delete[] cryptbuffer;
        return;
    }

    fsout.write((char*)cryptbuffer, bblk);

    fsout.close();
    fs.close();



    delete[] buffer;
    delete[] cryptbuffer;

#endif // CRY_ENCRYPT
}

void Application::decrypt(std::string file)
{
#if CRY_DECRYPT
    gcry_cipher_hd_t hd;

    gcry_error_t err = gcry_cipher_open(&hd, GPG_ALGO, GCRY_CIPHER_MODE_CBC, 0);

    size_t keylen = gcry_cipher_get_algo_keylen(GPG_ALGO);

    std::string key = cryptToLength("ozmodus9", keylen);

    size_t blklen = gcry_cipher_get_algo_blklen(GPG_ALGO);

    std::string blk = cryptToLength("testdata##9876", blklen);

    err = gcry_cipher_setkey(hd, key.c_str(), keylen);

    err = gcry_cipher_setiv(hd, blk.c_str(), blklen);

    fs::path p(file);

    if(!fs::exists(p)
            && !fs::is_regular_file(p))
    {
        return;
    }

    std::fstream fs;
    fs.open(p.string(), std::fstream::in | std::fstream::binary);

    std::stringstream ss;
    ss << file << ".d.txt";

    std::fstream fsout;
    fsout.open(ss.str(), std::fstream::out| std::fstream::binary);


    size_t cnt =1024 * 1024;//1Mb

    unsigned char *cryptbuffer = new unsigned char[cnt];
    unsigned char *buffer = new unsigned char[cnt];

    auto c = fs.readsome((char*)buffer,cnt);

    memset(cryptbuffer,0, cnt);

    err = gcry_cipher_decrypt(hd, cryptbuffer, c, buffer, c);

    fsout.write((char*)cryptbuffer,c);

    fsout.close();
    fs.close();



    delete[] buffer;
    delete[] cryptbuffer;
#endif // CRY_DECRYPT
}

std::string Application::cryptToLength(std::string in, size_t len)
{
    size_t l = in.size();

    if(l==len)
    {
        return in;
    }
    else if(l>len)
    {
        return std::string(in.begin(),in.begin()+len);
    }
    else
    {
        std::stringstream ss;
        ss << in;

        size_t i = 0;

        while(ss.str().size() < len)
        {
            ss << defualt_crypt_string[i];

            ++i;

            if(i>=strlen(defualt_crypt_string))
            {
                i = 0;
            }

        }

        return ss.str();
    }
}

void Application::identifyFile(std::string file)
{
    fs::path p(file);

    _fileName = "";

    if(fs::exists(p))
    {
        _fileName = file;
    }

    if(p.extension().string() == "CRY_EXT")
    {
        this->_encrypted = true;
    }

    _fileSize = fs::file_size(p);
}
