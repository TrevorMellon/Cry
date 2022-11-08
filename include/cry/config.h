
#pragma once

#include <gcrypt.h>
#include <stdint.h>

constexpr const size_t cry_buffer_multiplier    = 16;

constexpr const gcry_cipher_algos gpg_algo      = GCRY_CIPHER_SERPENT256;
constexpr const char* cry_ext                   = ".cry";

constexpr const char* cry_default_password      = "DEFAULTpassword123CHANGEME";
constexpr const char* cry_default_iv            = "cRy-b@by-369";

#include <cry/version.h>

constexpr const char* git_repo        = "git@github.com:TrevorMellon/Cry.git";
constexpr const char* git_branch      = "master";
constexpr const char* git_hash        = "c62c11d";
constexpr const uint32_t git_revision       = 104;

