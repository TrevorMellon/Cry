#ifndef CRY_COMMON_H
#define CRY_COMMON_H

#include <cry/config.h>
#include <iostream>
#include <sstream>
#include <gcrypt.h>
#include <stdint.h>
#include <vector>

namespace cry {

    using EncryptionType = gcry_cipher_algos;

    class CryHeader
    {
        public:

            // ==============================================

            CryHeader() = default;

            // ==============================================

            ~CryHeader() = default;

            // ==============================================

            uint64_t
            fileSize() const
            {
                return _filesize;
            }

            // ==============================================

            uint64_t
            filenameSize() const
            {
                return _filename.size();
            }

            // ==============================================

            const char *
            filename() const
            {
                return _filename.data();
            }

            // ==============================================

            size_t
            size() const
            {
                size_t ret = 0;
                ret += _filename.size();
                ret += 2 * sizeof ( uint64_t );
                ret += 2;
                return ret;
            }

            // ==============================================

            void
            clear()
            {
                _filename.clear();
                _filesize = 0;
            }

            // ==============================================

            void
            setFileSize ( const size_t fs )
            {
                _filesize = fs;
            }

            // ==============================================

            void
            setFilename ( const std::string &fn )
            {
                _filename = fn;
            }

            // ==============================================

            size_t
            readHeader ( std::vector<uint8_t> buffer )
            {
                const auto *ptr64 = ( uint64_t * ) buffer.data();
                setFileSize ( ptr64[0] );
                auto fns = ptr64[1];

                if ( fns >= FILENAME_MAX ) {
                    #if DBG
                    std::cerr << "Filename size too large" << std::endl;
                    #endif
                    this->clear();
                    return 0;
                }

                std::stringstream filenamess;
                const auto *ptr = buffer.data();
                ptr += 2 * sizeof ( uint64_t );

                if ( *ptr != '\n' ) {
                    return 0;
                }

                ++ptr;

                for ( size_t i = 0; i < fns; ++i ) {
                    filenamess << ( char ) ptr[i] ;
                }

                std::string s = filenamess.str();

                if ( s.size() != fns ) {
                    #if DBG
                    std::cerr << "Not a Cry encrypted file" << std::endl;
                    #endif
                    return 0;
                }

                this->setFilename ( s );
                const auto headersz = this->size();
                return headersz;
            }

            // ==============================================

            std::vector<uint8_t>
            writeHeader ()
            {
                std::vector<uint8_t> vec;
                vec.reserve ( size() );
                uint64_t tmp;
                tmp = _filesize;
                uint8_t *ptr = ( uint8_t * ) &tmp;

                for ( size_t i = 0; i < sizeof ( uint64_t ); ++i ) {
                    vec.push_back ( ptr[i] );
                }

                tmp = _filename.size();
                ptr = ( uint8_t * ) &tmp;

                for ( size_t i = 0; i < sizeof ( uint64_t ); ++i ) {
                    vec.push_back ( ptr[i] );
                }

                vec.push_back ( '\n' );

                for ( size_t i = 0; i < _filename.size(); ++i ) {
                    uint8_t cs = ( uint8_t ) _filename[i];
                    vec.push_back ( cs );
                }

                *ptr = 0;
                return vec;
            }

            // ==============================================

        private:
            uint64_t           _filesize = 0;
            std::string        _filename;

    };

} // namespace cry
#endif // CRY_COMMON_H
