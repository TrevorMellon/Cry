
#ifndef CRY_H
#define CRY_H

#include <cry/config.h>
#include <cry/common.h>

#include <string>

namespace cry { 

    class CryPrivate;
    
    class Cry 
    {
    public:
        Cry();
        ~Cry();        
    public:
        void encrypt(std::string file, EncryptionType type=GPG_ALGO);
        void decrypt(std::string file, EncryptionType type=GPG_ALGO);
        void setOutput(std::string output);
        void setPassword(std::string password);
        
    private:
        CryPrivate *_p;
    };
}

#endif // CRY_H
