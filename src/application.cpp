
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

// ==========================================================================

Application::Application() 
    : _fileName(""),
      _fileSize(0),
      _encrypted(false),
      _originalFileName(""),
      _password(CRY_DEFAULT_PASSWORD)
{

}

// ==========================================================================

Application::~Application()
{
    
}

// ==========================================================================

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

// ==========================================================================

void Application::encrypt(std::string file, EncryptionType type)
{
#if CRY_ENCRYPT
    encryptImpl(file, type);
#endif
}

void Application::decrypt(std::string file, EncryptionType type)
{
#if CRY_DECRYPT
    decryptImpl(file, type);
#endif
}

// ==========================================================================

void Application::encryptImpl(std::string file, EncryptionType type)
{
#if CRY_ENCRYPT
    identifyFile(file);

    gcry_cipher_hd_t hd;

    gcry_error_t err = gcry_cipher_open(&hd, type, GCRY_CIPHER_MODE_CBC, 0);

    CryptDetail cd = getCryptDetails(type);

    err = gcry_cipher_setkey(hd, cd.key.c_str(), cd.keylength);

    err = gcry_cipher_setiv(hd, cd.block.c_str(), cd.blocklength);

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


    size_t cnt =1024 * cd.blocklength;

    CryHeader hdr;
    hdr.filesize = _fileSize;
    hdr.filenamesize = file.size();
    hdr.filename = new char[hdr.filenamesize+1];

    memset(hdr.filename,0,hdr.filesize+1);
    char *fnptr = hdr.filename;
    for(char cc: file)
    {
        *fnptr = cc;
        ++fnptr;
    }
    *fnptr = 0;

    size_t hdrsz = sizeof(uint64_t)*2 + hdr.filenamesize + 2;

    unsigned char *buffer = new unsigned char[cnt];
    unsigned char *sbuff = new unsigned char[cnt];

    memset(buffer,0,cnt);

    auto some = fs.readsome((char*)buffer,cnt-hdrsz);

    size_t sz = writeHeader(sbuff, hdr);

    if(sz <= 0)
    {
#if DBG
        std::cerr << "Unable to write header" << std::endl;
#endif
        return;
    }

    uint8_t *ptr = sbuff;
    ptr += hdrsz;

    for (size_t i = 0; i < some; ++i)
    {
        *ptr = buffer[i];
        ++ptr;
    }

    size_t bblk = some + hdrsz;

    size_t mult = bblk / cd.blocklength;
    size_t remainder = bblk % cd.blocklength;

    size_t r = mult;
    remainder? ++r:r;

    if(cd.blocklength>cnt)
    {
#if DBG
        std::cerr << "Buffer not large enough" << std::endl;
#endif
        return;
    }

    size_t outcnt = r * cd.blocklength;

    unsigned char *cryptbuffer = new unsigned char[outcnt];

    memset(cryptbuffer,0, cnt);

    err = gcry_cipher_encrypt(hd, cryptbuffer, cnt, sbuff, cnt);

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

// ==========================================================================

void Application::decryptImpl(std::string file, EncryptionType type)
{
#if CRY_DECRYPT
    gcry_cipher_hd_t hd;

    gcry_error_t err = gcry_cipher_open(&hd, type, GCRY_CIPHER_MODE_CBC, 0);

    CryptDetail cd = getCryptDetails(type);

    err = gcry_cipher_setkey(hd, cd.key.c_str(), cd.keylength);

    err = gcry_cipher_setiv(hd, cd.block.c_str(), cd.blocklength);

    fs::path p(file);

    if(!fs::exists(p)
            && !fs::is_regular_file(p))
    {
        return;
    }

    std::fstream fs;
    fs.open(p.string(), std::fstream::in | std::fstream::binary);

    size_t cnt =1024 * cd.blocklength;

    unsigned char *cryptbuffer = new unsigned char[cnt];
    unsigned char *buffer = new unsigned char[cnt];

    auto c = fs.readsome((char*)buffer,cnt);    

    memset(cryptbuffer,0, cnt);

    err = gcry_cipher_decrypt(hd, cryptbuffer, c, buffer, c);

    CryHeader ch;
    size_t sz = readHeader(cryptbuffer, &ch);

    char *cptr = (char*)cryptbuffer;
    cptr += sizeof(uint64_t)+2 + ch.filenamesize + 2;

    std::fstream fsout;
    fsout.open(ch.filename, std::fstream::out| std::fstream::binary);

    fsout.write(cptr, _fileSize);

    fsout.close();
    fs.close();



    delete[] buffer;
    delete[] cryptbuffer;
#endif // CRY_DECRYPT
}

// ==========================================================================

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

// ==========================================================================

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

// ==========================================================================

CryptDetail Application::getCryptDetails(EncryptionType type)
{
    CryptDetail c;

    c.keylength = gcry_cipher_get_algo_keylen(type);

    c.key = cryptToLength(_password, c.keylength);

    c.blocklength = gcry_cipher_get_algo_blklen(type);

    std::string blk = cryptToLength("testdata##9876", c.blocklength);

    return c;
}

// ==========================================================================

size_t Application::writeHeader(unsigned char* buffer, const CryHeader &hdr)
{
    uint64_t *ptr64 = (uint64_t*)buffer;
    ptr64[0] = hdr.filesize;
    ptr64[1] = hdr.filenamesize;

    uint8_t *ptr = buffer;
    ptr += (2*sizeof(uint64_t));
    *ptr = '\n';
    ++ptr;

    for(size_t i=0;i<hdr.filenamesize;++i)
    {
        uint8_t cs = (uint8_t)hdr.filename[i];
        *ptr = cs;
        ++ptr;
    }
    *ptr = 0;

    size_t sz = sizeof(uint64_t)*2 + 2 + hdr.filenamesize;

    return sz;
}

// ==========================================================================

size_t Application::readHeader(const unsigned char* buffer, CryHeader *headr)
{
    CryHeader &hdr = *headr;

    uint64_t *ptr64 = (uint64_t*)buffer;
    hdr.filesize = ptr64[0];
    hdr.filenamesize = ptr64[1];

    if(hdr.filenamesize >= FILENAME_MAX)
    {
#if DBG
        std::cerr << "Filename size too large" << std::endl;
#endif
        return -1;
    }

    hdr.filename = new char[hdr.filenamesize+1];

    uint8_t *ptr = (uint8_t*)buffer;
    ptr += 2 * sizeof(uint64_t);

    for(size_t i = 0; i<hdr.filenamesize; ++i)
    {
        hdr.filename[i] = *(ptr + i);
    }
    hdr.filename[hdr.filenamesize] = 0;



    if(strlen(hdr.filename) != hdr.filenamesize)
    {
#if DBG
        std::cerr << "Not a Cry encrypted file" << std::endl;
#endif
        delete[] hdr.filename;
        hdr.filename = nullptr;
        return -1;
    }

    uint64_t headersz = 2 * sizeof(uint64_t);
    headersz += _fileName.size() + 2;

    return headersz;
}
