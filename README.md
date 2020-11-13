"cry" is a command line app for encrypting large archives, specifically without prompting for a password.

# Requirements

* cmake
* C++ compiler
* Boost libraries
* gcrypt library

# Install

There are three specific options for compilation, mutually exclusive

* BUILD_ENC    - Build the encrypter
* BUILD_DEC    - Build the decrypter
* BUILD_ENCDEC - Build both the encrypter and decrypter in the same library

Specifying the default password is done with:

* CRY_DEFAULT_PASSWORD

The initialisation vector can be set with:

* CRY_DEFAULT_IV

Both those to variables should be specified for production and stored separately securely.

# Building

* cmake -DBUILD_ENCDEC -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr .
* make
* make install
