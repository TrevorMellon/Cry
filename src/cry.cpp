
#include <cry/cry.h>
#include <cry/config.h>

#include <gcrypt.h>

#include <iostream>
#include <sstream>
#include <fstream>

#include <boost/filesystem.hpp>

#include <math.h>

namespace fs = boost::filesystem;

namespace cry {
    const char *defualt_crypt_string = cry_default_iv;

    //***************************************************************************************
    //
    //                                      CRY
    //
    //***************************************************************************************

    Cry::Cry()
    {
    }

    //===========================================================

    void
    Cry::encrypt ( std::string file, EncryptionType type )
    {
        #if CRY_ENCRYPT
        encryptImpl ( file, type );
        #endif
    }

    //===========================================================

    void
    Cry::decrypt ( std::string file, EncryptionType type )
    {
        #if CRY_DECRYPT
        decryptImpl ( file, type );
        #endif
    }

    //===========================================================

    void
    Cry::setOutput ( std::string output )
    {
        _output = output;
    }

    //===========================================================

    void
    Cry::setPassword ( std::string password )
    {
        _password = password;
    }

    //===========================================================

    void
    Cry::encryptImpl ( std::string file, EncryptionType type )
    {
        #if CRY_ENCRYPT
        gcry_cipher_hd_t hd;
        size_t filesize = identifyFile ( file );

        if ( filesize == 0 ) {
            #if DBG
            std::cerr << "Could not find file" << std::endl;
            #endif
            return;
        }

        CryptDetail cd = encryptionSetup (
                hd,
                type
            );
        fs::path p ( file );

        if ( !fs::exists ( p )
            && !fs::is_regular_file ( p ) ) {
            return;
        }

        std::fstream fsin;
        fsin.open (
            p.string(),
            std::fstream::in | std::fstream::binary );
        std::stringstream ss;
        ss << file << cry_ext;
        std::fstream fsout;
        fsout.open (
            ss.str(),
            std::fstream::out | std::fstream::binary );
        CryHeader hdr;
        hdr.setFileSize ( filesize );
        hdr.setFilename ( file );
        auto h = hdr.writeHeader ();

        if ( h.size() <= 0 ) {
            #if DBG
            std::cerr << "Unable to write header" << std::endl;
            #endif
            return;
        }

        auto hp = padToBlock (
                h,
                cd.blocklength );
        auto block = encryptBlock (
                hp,
                cd.blocklength,
                hd
            );
        uint16_t blocks = block.size() / cd.blocklength;
        const char *magic = "#CRY2";
        fsout.write ( magic, 5 );
        fsout.write ( reinterpret_cast<char *> ( &blocks ), sizeof ( uint16_t ) );
        fsout.write ( ( char * ) block.data(), block.size() );

        if ( filesize < cd.blocklength ) {
            char buff[filesize];
            fsin.read ( buff, filesize );
            std::vector<uint8_t> buffvec;
            buffvec.reserve ( filesize );
            buffvec.assign ( buff, buff + filesize );
            auto pad = padToBlock ( buffvec, cd.blocklength );
            auto enc = encryptBlock ( pad, cd.blocklength, hd );
        } else {
            int64_t store = filesize;

            while ( store > 0 ) {
                if ( store < cd.blocklength ) {
                    char buff[store];
                    fsin.read ( buff, store );
                    std::vector<uint8_t> buffvec;
                    buffvec.reserve ( store );
                    buffvec.assign ( buff, buff + store );
                    auto pad = padToBlock ( buffvec, cd.blocklength );
                    auto enc = encryptBlock ( pad, cd.blocklength, hd );
                    fsout.write ( ( char * ) enc.data(), enc.size() );
                } else {
                    char buff[cd.blocklength];
                    fsin.read ( buff, cd.blocklength );
                    std::vector<uint8_t> buffvec;
                    buffvec.reserve ( cd.blocklength );
                    buffvec.assign ( buff, buff + cd.blocklength );
                    //auto pad = padToBlock ( buffvec, cd.blocklength );
                    auto enc = encryptBlock ( buffvec, cd.blocklength, hd );
                    fsout.write ( ( char * ) enc.data(), enc.size() );
                }

                store -= cd.blocklength;
            }
        }

        fsout.close();
        fsin.close();
        #endif // CRY_ENCRYPT
    }

    void
    Cry::decryptImpl (
        std::string file,
        EncryptionType type )
    {
        #if CRY_DECRYPT
        gcry_cipher_hd_t hd;
        auto cd = encryptionSetup (
                hd,
                type
            );
        fs::path p ( file );

        if ( !fs::exists ( p )
            && !fs::is_regular_file ( p ) ) {
            return;
        }

        std::fstream fs;
        fs.open ( p.string(), std::fstream::in | std::fstream::binary );
        std::vector<uint8_t> cryptbuffer;
        size_t filesz = fs::file_size ( file );

        if ( filesz == 0 ) {
            #if DBG
            std::cerr << "Cry file empty" << std::endl;
            #endif
            return;
        }

        char magic[6];
        fs.read ( magic, 5 );
        magic[5] = '\0';

        if ( strncmp ( "#CRY2", magic, 4 ) != 0 ) {
            #if DBG
            std::cerr << "Not a cry file" << std::endl;
            #endif
            return;
        }

        uint16_t b = 0;
        fs.read ( reinterpret_cast<char *> ( &b ), sizeof ( uint16_t ) );
        size_t bsz = cd.blocklength * b;
        char *buffin = new char[bsz];
        fs.read ( buffin, bsz );
        char buffdec[bsz];
        gcry_error_t err = gcry_cipher_decrypt ( hd, buffdec, bsz, buffin, bsz );
        delete[] buffin;
        buffin = nullptr;
        cryptbuffer.reserve ( bsz );
        cryptbuffer.assign ( buffdec, buffdec + ( bsz ) );

        if ( err > 0 ) {
            auto e = gcry_err_code ( err );
            #if DBG
            std::cerr << "Could not decrypt - error code: " << e << std::endl;
            #endif
        }

        CryHeader ch;
        size_t sz = ch.readHeader ( cryptbuffer );

        if ( sz <= 0 ) {
            #if DBG
            std::cerr << "Could not read header" << std::endl;
            #endif
            return;
        }

        std::stringstream ssout;
        //Filename of output file
        ssout << std::string ( ch.filename(), ch.filenameSize() );

        if ( fs::exists ( ssout.str() ) ) {
            std::cout << "File: \"" << ssout.str() << " already exists!" << std::endl;
            std::cout << "Do you want me to overwrite? (Y/N)" << std::endl;
            std::string input;
            std::cin >> input;

            if ( input == "Y" || input == "y" ) {
                //do nothing
            } else {
                ssout << ".out";
            }
        }

        std::fstream fsout;

        if ( _output.empty() ) {
            fsout.open ( ssout.str(), std::fstream::out | std::fstream::binary );
        } else {
            fsout.open ( _output, std::fstream::out | std::fstream::binary );
        }

        if ( fsout.fail() || fsout.bad() ) {
            #if DBG
            std::cerr << "Could not open: " << ssout.str() << std::endl;
            #endif
        }

        size_t filesizecount = ch.fileSize();
        float div = float ( filesizecount ) / float ( cd.blocklength );
        size_t blocks = ceil ( div );

        while ( blocks > 0 ) {
            if ( filesizecount < cd.blocklength ) {
                char buffdec[cd.blocklength];
                char buffin[cd.blocklength];
                fs.read ( buffin, cd.blocklength );
                err = gcry_cipher_decrypt ( hd, buffdec, cd.blocklength, buffin, cd.blocklength );

                if ( err > 0 ) {
                    #if DBG
                    auto ec = gcry_err_code ( err );
                    std::cerr << "Error while decrypting: " << ec << std::endl;
                    #endif
                } else {
                    fsout.write ( buffdec, filesizecount );
                }
            } else {
                char buffdec[cd.blocklength];
                char buffin[cd.blocklength];
                fs.read ( buffin, cd.blocklength );
                err = gcry_cipher_decrypt ( hd, buffdec, cd.blocklength, buffin, cd.blocklength );

                if ( err > 0 ) {
                    #if DBG
                    auto ec = gcry_err_code ( err );
                    std::cerr << "Error while decrypting: " << ec << std::endl;
                    #endif
                } else {
                    fsout.write ( buffdec, cd.blocklength );
                }
            }

            --blocks;
            filesizecount -= cd.blocklength;
        }

        fsout.close();
        fs.close();
        #endif // CRY_DECRYPT
    }

    size_t
    Cry::identifyFile ( std::string file )
    {
        fs::path p ( file );
        size_t filesize = 0;

        if ( !fs::exists ( p ) ) {
            return 0;
        }

        filesize = fs::file_size ( p );
        return filesize;
    }

    CryptDetail
    Cry::getCryptDetails ( EncryptionType type )
    {
        CryptDetail c;
        c.keylength = gcry_cipher_get_algo_keylen ( type );
        c.key = cryptToLength ( _password, c.keylength );
        c.blocklength = gcry_cipher_get_algo_blklen ( type );
        c.block = cryptToLength ( cry_default_iv, c.blocklength );
        return c;
    }

    std::string
    Cry::cryptToLength ( std::string in, size_t len )
    {
        size_t l = in.size();

        if ( l == len ) {
            return in;
        } else if ( l > len ) {
            return std::string ( in.begin(), in.begin() + len );
        } else {
            std::stringstream ss;
            ss << in;
            size_t i = 0;

            while ( ss.str().size() < len ) {
                ss << defualt_crypt_string[i];
                ++i;

                if ( i >= strlen ( defualt_crypt_string ) ) {
                    i = 0;
                }
            }

            return ss.str();
        }
    }

    CryptDetail
    Cry::encryptionSetup ( gcry_cipher_hd_t &hd, EncryptionType type )
    {
        //gcry_cipher_hd_t hd;
        gcry_error_t err = gcry_cipher_open ( &hd, type, GCRY_CIPHER_MODE_CBC, 0 );

        if ( err != GPG_ERR_NO_ERROR ) {
            #if DBG
            std::cerr << "Couldn't open cipher: " << std::endl;
            std::cerr << "Code: " << err << std::endl;
            #endif
            exit ( 1 );
        }

        CryptDetail cd = getCryptDetails ( type );
        err = gcry_cipher_setkey ( hd, cd.key.c_str(), cd.keylength );

        if ( err != GPG_ERR_NO_ERROR ) {
            #if DBG
            std::cerr << "Couldn't set encryption key: " << std::endl;
            std::cerr << "Code: " << err << std::endl;
            #endif
            exit ( 1 );
        }

        err = gcry_cipher_setiv ( hd, cd.block.c_str(), cd.blocklength );

        if ( err != GPG_ERR_NO_ERROR ) {
            #if DBG
            std::cerr << "Couldn't set IV: " << std::endl;
            std::cerr << "Code: " << err << std::endl;
            #endif
            exit ( 1 );
        }

        return cd;
    }

    std::vector<uint8_t>
    Cry::encryptBlock (
        const std::vector<uint8_t> &bufferin,
        const size_t blockSize,
        gcry_cipher_hd_t &hd )
    {
        std::vector<uint8_t> outvec;
        size_t sz = bufferin.size();
        uint8_t *out = new uint8_t[sz];
        gcry_error_t err = gcry_cipher_encrypt ( hd, out, sz, bufferin.data(), sz );

        if ( err ) {
            #if DBG
            auto ec = gcry_err_code ( err );
            std::cerr << "Gcrypt Error: " << ec << std::endl;
            #endif
            return outvec;
        }

        outvec.assign ( out, out + sz );
        delete[] out;
        return outvec;
    }

    std::vector<uint8_t>
    Cry::padToBlock (
        const std::vector<uint8_t> &in,
        const int blocksize
    )
    {
        std::vector<uint8_t> outvec;

        if ( in.size() <= blocksize ) {
            outvec.reserve ( blocksize );
            outvec.assign ( in.begin(), in.end() );

            while ( outvec.size() < blocksize ) {
                outvec.push_back ( 0 );
            }
        } else {
            float div = float ( in.size() ) / float ( blocksize );
            int count = ceil ( div );
            size_t sz = blocksize * count;
            outvec.reserve ( count );
            outvec.assign ( in.begin(), in.end() );

            while ( outvec.size() < sz ) {
                outvec.push_back ( 0 );
            }
        }

        return outvec;
    }

} // namespace Cry
