
#include <application.h>

#include <iostream>
#include <string>
#include <vector>
#include <fstream>

#include <gcrypt.h>

#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>

#include <config.h>

const char* defualt_crypt_string = CRY_DEFAULT_IV;

namespace po = boost::program_options;
namespace fs = boost::filesystem;

using namespace cry;

struct CryHeader
{
    uint64_t    filesize;
    uint64_t    filenamesize;
};

Application::Application() 
    : _fileName(""),
      _fileSize(0),
      _encrypted(false),
      _originalFileName(""),
      _password(CRY_DEFAULT_PASSWORD)
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
#if CRY_DECRYPT || CRY_ENCRYPT
            ("password,p", po::value<std::string>(), "set the given password")
#endif
            ("input,f", po::value<std::string>(), "input file")
            ;
    po::positional_options_description p;
    p.add("input", -1);
    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv).options(d).positional(p).run(), vm);
    po::notify(vm);

#if CRY_DECRYPT || CRY_ENCRYPT
    if(vm.count("password"))
    {
#if DBG
        std::cout << "Using Password: " << vm["password"].as<std::string>() << std::endl;
#endif
        _password = vm["password"].as<std::string>();
    }
#endif
    if(vm.count("encrypt")
            && vm.count("input"))
    {
#if CRY_ENCRYPT
# if DBG
        std::cout << "Encrypt \"" << vm["input"].as< std::string >() << "\"" << std::endl;
# endif//DBG
        encrypt(vm["input"].as< std::string >());
        //e
#endif
    }
    else if(vm.count("decrypt")
            && vm.count("input"))
    {
#if CRY_DECRYPT
# if DBG
        std::cout << "Decrypt \"" << vm["input"].as< std::string>() << "\"" << std::endl;
# endif  //DBG
        //d
        decrypt(vm["input"].as<std::string>());
#endif
    }

}

void Application::encrypt(std::string file, EncryptionType type)
{
    encryptImpl(file, type);
}

void Application::decrypt(std::string file, EncryptionType type)
{
    decryptImpl(file, type);
}

void Application::encryptImpl(std::string file, EncryptionType type)
{
#if CRY_ENCRYPT
    identifyFile(file);

    gcry_cipher_hd_t hd;

    gcry_error_t err = gcry_cipher_open(&hd, GPG_ALGO, GCRY_CIPHER_MODE_CBC, 0);

    size_t keylen = gcry_cipher_get_algo_keylen(type);

    std::string key = cryptToLength(_password, keylen);

    size_t blklen = gcry_cipher_get_algo_blklen(type);

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
    ss << file << CRY_EXT;

    std::fstream fsout;
    fsout.open(ss.str(), std::fstream::out| std::fstream::binary);


    size_t cnt =1024 * blklen;//1Mb

    CryHeader hdr;
    hdr.filesize = _fileSize;
    hdr.filenamesize = file.size();

    uint64_t headersz = sizeof(hdr)+file.size()+2;

    unsigned char *buffer = new unsigned char[cnt];
    unsigned char* sbuff = new unsigned char[cnt];
    memset(buffer,0,cnt);

    auto c = fs.readsome((char*)buffer,cnt-headersz);

    //hdrss << _fileSize << file.size();
    //hdrss << '\n' << file << '\0';
    //hdrss << buffer;

    uint64_t *ptr64 = (uint64_t*)sbuff;
    ptr64[0] = _fileSize;
    ptr64[1] = file.size();

    uint8_t *ptr = sbuff;
    ptr += (2*sizeof(uint64_t));
    *ptr = '\n';
    ++ptr;

    for(char cs: file)
    {
        *ptr = (uint8_t)cs;
        ++ptr;
    }
    *ptr = 0;
    ++ptr;
    for(int i=0;i<c;++i)
    {
        *ptr = buffer[i];
        ++ptr;
    }

    unsigned char *cryptbuffer = new unsigned char[cnt];

    c += headersz;

    int rem = c % blklen;
    int r = c / blklen;

    if(rem)
    {
        ++r;
    }

    size_t bblk = r * blklen;

    memset(cryptbuffer,0, cnt);

    err = gcry_cipher_encrypt(hd, cryptbuffer, bblk, sbuff, bblk);

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
    delete[] sbuff;

    //decrypt(ss.str());

#endif // CRY_ENCRYPT
}

void Application::decryptImpl(std::string file, EncryptionType type)
{
#if CRY_DECRYPT
    gcry_cipher_hd_t hd;

    gcry_error_t err = gcry_cipher_open(&hd, GPG_ALGO, GCRY_CIPHER_MODE_CBC, 0);

    size_t keylen = gcry_cipher_get_algo_keylen(type);

    std::string key = cryptToLength(_password, keylen);

    size_t blklen = gcry_cipher_get_algo_blklen(type);

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

    size_t cnt =1024 * blklen;//1Mb

    unsigned char *cryptbuffer = new unsigned char[cnt];
    unsigned char *buffer = new unsigned char[cnt];

    auto c = fs.readsome((char*)buffer,cnt);    

    memset(cryptbuffer,0, cnt);

    err = gcry_cipher_decrypt(hd, cryptbuffer, c, buffer, c);

    uint64_t filenameSize;

    uint64_t *ptr64 = (uint64_t*)cryptbuffer;
    _fileSize = ptr64[0];
    filenameSize = ptr64[1];

    if(filenameSize >= FILENAME_MAX)
    {
#if DBG
        std::cerr << "Filename size too large" << std::endl;
#endif
        return;
    }

    uint8_t* ptr = cryptbuffer;
    ptr += 2 * sizeof(uint64_t);

    char* cptr = (char*)ptr;
    if(*cptr != '\n')
    {
        return;
    }
    ++ptr;

    std::stringstream nm;

    cptr = (char*)ptr;

    nm << cptr;

    ptr+=nm.str().size()+1;
    cptr = (char*)ptr;

    _fileName = nm.str();

    //std::stringstream fss;
    //fss << cptr;

    if(_fileName.size() != filenameSize)
    {
#if DBG
        std::cerr << "Not a Cry encrypted file" << std::endl;
#endif
        return;
    }

    uint64_t headersz = 2 * sizeof(uint64_t);
    headersz += _fileName.size() + 2;

    ptr = cryptbuffer + headersz;

    nm << ".out";

    std::fstream fsout;
    fsout.open(nm.str(), std::fstream::out| std::fstream::binary);

    fsout.write(cptr, _fileSize);

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
