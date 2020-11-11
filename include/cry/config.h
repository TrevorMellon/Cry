#ifndef CONFIG_H
#define CONFIG_H

#include <gcrypt.h>

#define  CRY_BUFFER_MULT 16 

#define GPG_ALGO GCRY_CIPHER_SERPENT256
#define CRY_EXT ".cry"

#include <cry/version.h>

#define GIT_REPO       ssh://git@git.ignifi.me:10022/deadman/cry.git
#define GIT_BRANCH     libcry
#define GIT_COMMIT_HASH    0f3e3ee
#define GIT_REV_COUNT  95

#define CRY_DEFAULT_PASSWORD "DEFAULTpassword123CHANGEME"
#define CRY_DEFAULT_IV "cRy-b@by-369"

#endif // CONFIG_H
