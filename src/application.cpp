
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
    : _password(CRY_DEFAULT_PASSWORD)
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
    size_t filesize = identifyFile(file);

    if(filesize==0)
    {
#if DBG
        std::cerr << "Could not find file" << std::endl;
#endif
        return;
    }

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


    size_t cnt =CRY_BUFFER_MULT * cd.blocklength;

    CryHeader hdr;
    hdr.filesize = filesize;
    hdr.filenamesize = file.size();
    hdr.filename = new char[hdr.filenamesize+1];

    memset(hdr.filename,0,hdr.filenamesize+1);
    char *fnptr = hdr.filename;
    for(char cc: file)
    {
        *fnptr = cc;
        ++fnptr;
    }
    *fnptr = 0;

    size_t hdrsz = sizeof(uint64_t)*2 + hdr.filenamesize + 2;

    unsigned char *filebuffer = new unsigned char[cnt];
    unsigned char *sbuff = new unsigned char[cnt];

    memset(sbuff,0,cnt);

    size_t sz = writeHeader(sbuff, hdr);

    size_t buffer_count = cnt;

    if((int64_t)hdr.filesize-(int64_t)hdrsz < (int64_t)cnt)
    {
        buffer_count = hdr.filesize;

        fs.read((char*)filebuffer,buffer_count);
    }
    else
    {
        buffer_count = buffer_count-sz;

        fs.read((char*)filebuffer,buffer_count);
    }

    if(sz <= 0)
    {
#if DBG
        std::cerr << "Unable to write header" << std::endl;
#endif
        return;
    }

    uint8_t *ptr = sbuff;
    ptr += hdrsz;

    for (size_t i = 0; i < buffer_count; ++i) //fill from file contents
    {
        *ptr = filebuffer[i];
        ++ptr;
    }

    size_t bblk = buffer_count + sz;

    size_t mult = bblk / cd.blocklength;
    size_t remainder = bblk % cd.blocklength;

    size_t r = mult;
    remainder? ++r:r;

    size_t outcnt = r * cd.blocklength;

    if(outcnt>cnt)
    {
#if DBG
        std::cerr << "Buffer not large enough" << std::endl;
#endif
        return;
    }

    unsigned char *cryptbuffer = new unsigned char[cnt];
    memset(cryptbuffer,0, cnt);

    err = gcry_cipher_encrypt(hd, cryptbuffer, outcnt, sbuff, outcnt);

    if(err)
    {
#if DBG
        auto ec = gcry_err_code(err);
        std::cerr << "Gcrypt Error: " << ec << std::endl;
#endif
        delete[] filebuffer;
        delete[] cryptbuffer;
        delete[] sbuff;
        return;
    }

    fsout.write((char*)cryptbuffer, outcnt);

    int64_t fileoutcount = hdr.filesize - buffer_count - hdrsz;

    while(fileoutcount > 0)
    {
        buffer_count = cnt;

        if(fileoutcount < cnt)
        {
            buffer_count = fileoutcount;
        }

        fs.read((char*)filebuffer, buffer_count);


        memset(cryptbuffer,0,cnt);

        bblk = buffer_count;

        mult = bblk / cd.blocklength;
        remainder = bblk % cd.blocklength;

        r = mult;
        remainder? ++r:r;

        outcnt = r * cd.blocklength;

        if(r==0)
        {
            break;
        }

        if(remainder != 0)
        {
#if DBG
            std::cout << "remainder not 0, filesize remaining: " << fileoutcount;
#endif
        }

        if(outcnt>cnt)
        {
    #if DBG
            std::cerr << "Buffer not large enough" << std::endl;
    #endif
            delete[] filebuffer;
            if(cryptbuffer)
            {
                delete[] cryptbuffer;
                cryptbuffer = nullptr;
            }
            delete[] sbuff;
            return;
        }

        memset(cryptbuffer,0, cnt);

        err = gcry_cipher_encrypt(hd, cryptbuffer, outcnt, filebuffer, outcnt);

        if(err)
        {
#if DBG
            auto ec = gcry_err_code(err);
            std::cerr << "Gcrypt Error: " << ec << std::endl;
#endif
            delete[] filebuffer;
            if(cryptbuffer)
            {
                delete[] cryptbuffer;
                cryptbuffer = nullptr;
            }
            delete[] sbuff;
            return;
        }

        fsout.write((char*)cryptbuffer, outcnt);

        fileoutcount -= buffer_count;
    }

    fsout.close();
    fs.close();

    delete[] filebuffer;
    if(cryptbuffer)
    {
        delete[] cryptbuffer;
        cryptbuffer = nullptr;
    }
    delete[] sbuff;

#if DBG
    decrypt(ss.str());
#endif

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

    size_t cnt =CRY_BUFFER_MULT * cd.blocklength;

    unsigned char *cryptbuffer = new unsigned char[cnt];
    unsigned char *buffer = new unsigned char[cnt];

    memset(buffer,0,cnt);

    size_t buffer_count = cnt;

    size_t filesz = fs::file_size(file);

    if (filesz <= cnt)
    {
        buffer_count = filesz;
    }

    fs.read((char*)buffer, buffer_count);

    memset(cryptbuffer,0, cnt);

    err = gcry_cipher_decrypt(hd, cryptbuffer, buffer_count, buffer, buffer_count);

    if(err > 0)
    {
        auto e = gcry_err_code(err);
#if DBG
        std::cerr << "Could not decrypt - error code: " << e << std::endl;
#endif
    }

    CryHeader ch;
    size_t sz = readHeader(cryptbuffer, &ch);

    buffer_count -= sz;

    if(sz<=0)
    {
#if DBG
        std::cerr << "Could not read header" << std::endl;
#endif
        return;
    }

    if(ch.filesize < buffer_count)
    {
        buffer_count = ch.filesize;
    }

    char *cptr = (char*)cryptbuffer;
    cptr += sz;

    std::stringstream ssout;
    ssout << ch.filename << ".out";

    std::fstream fsout;

    fsout.open(ssout.str(), std::fstream::out| std::fstream::binary);

    fsout.write(cptr, buffer_count);

    cptr += buffer_count;


    int64_t filesizecount = ch.filesize - buffer_count;

    while(filesizecount > 0)
    {
        if(filesizecount>cnt)
        {
            buffer_count = cnt;
        }
        else
        {
            buffer_count = filesizecount;

            size_t bblk = buffer_count;

            size_t mult = bblk / cd.blocklength;
            size_t remainder = bblk % cd.blocklength;

            size_t r = mult;
            remainder? ++r:r;

            buffer_count = r * cd.blocklength;
        }

        fs.read((char*)buffer,buffer_count);

        memset(cryptbuffer,0, buffer_count);

        err = gcry_cipher_decrypt(hd, cryptbuffer, buffer_count, buffer, buffer_count);

        if(err>0)
        {
#if DBG
            auto ec = gcry_err_code(err);
            std::cerr << "Error while decrypting: " << ec << std::endl;
#endif
        }

        if(filesizecount>=buffer_count)
        {
            fsout.write((char*)cryptbuffer, buffer_count);
        }
        else
        {
            fsout.write((char*)cryptbuffer, filesizecount);
        }

        filesizecount -= buffer_count;
    }

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

size_t Application::identifyFile(std::string file)
{
    fs::path p(file);

    size_t filesize = 0;

    if(!fs::exists(p))
    {
        return 0;
    }    

    filesize = fs::file_size(p);
    return filesize;
}

// ==========================================================================

CryptDetail Application::getCryptDetails(EncryptionType type)
{
    CryptDetail c;

    c.keylength = gcry_cipher_get_algo_keylen(type);

    c.key = cryptToLength(_password, c.keylength);

    c.blocklength = gcry_cipher_get_algo_blklen(type);

    c.block = cryptToLength("testdata##9876", c.blocklength);

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
    CryHeader *hdr = headr;

    uint64_t *ptr64 = (uint64_t*)buffer;
    hdr->filesize = ptr64[0];
    hdr->filenamesize = ptr64[1];

    if(hdr->filenamesize >= FILENAME_MAX)
    {
#if DBG
        std::cerr << "Filename size too large" << std::endl;
#endif
        memset(headr, 0, sizeof(CryHeader));
        return 0;
    }

    hdr->filename = new char[hdr->filenamesize+1];

    uint8_t *ptr = (uint8_t*)buffer;
    ptr += 2 * sizeof(uint64_t);
    if(*ptr != '\n')
    {
        delete[] hdr->filename;
        hdr->filename = nullptr;
        return 0;
    }
    ++ptr;

    for(size_t i = 0; i<hdr->filenamesize; ++i)
    {
        hdr->filename[i] = *(ptr + i);
    }
    hdr->filename[hdr->filenamesize] = 0;

    if(strlen(hdr->filename) != hdr->filenamesize)
    {
#if DBG
        std::cerr << "Not a Cry encrypted file" << std::endl;
#endif
        delete[] hdr->filename;
        hdr->filename = nullptr;
        return 0;
    }

    uint64_t headersz = 2 * sizeof(uint64_t);
    headersz += strlen(hdr->filename) + 2;

    return headersz;
}
