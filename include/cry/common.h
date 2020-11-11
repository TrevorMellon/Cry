#ifndef CRY_COMMON_H
#define CRY_COMMON_H

#include <gcrypt.h>
#include <stdint.h>

namespace cry
{

    typedef gcry_cipher_algos EncryptionType;

    class CryHeader
    {
        public:
            uint64_t    filesize;
            uint64_t    filenamesize;
            char       *filename;

            CryHeader()
            {
                filename = nullptr;
                filenamesize = 0;
                filesize = 0;
            }

            ~CryHeader()
            {
                if ( filename )
                {
                    delete[] filename;
                    filename = nullptr;
                }
            }

    };

} // namespace cry
#endif // CRY_COMMON_H
