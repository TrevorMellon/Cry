"cry" is a command line app for encrypting large archives, specifically without prompting for a password.
It separates the encryptor and the decryptor into individual applications, so a server can encrypt files but not decrypt for example.

It will hopefully be useful as a reference to integrate gpg encryption into an application.

# Warning

Don't use this in production, the same can be done with native gpg and public/private keys. This was just a learning excercise.

# Requirements

* cmake
* C++ compiler
* Boost libraries
* gcrypt library

# Install

There are three specific options for compilation, mutually exclusive

* BUILD_ENC    - Build the encrypter
* BUILD_DEC    - Build the decrypter
* BUILD_ENCDEC - Build both the encrypter and decrypter in the same application

Specifying the default password is done with:

* CRY_DEFAULT_PASSWORD

# Building

* cmake -DBUILD_ENCDEC -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr .
* make
* make install
