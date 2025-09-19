/*
 *
 *
 * ⠀⠈⢿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠿⠿⠿⠛⠛⠛⠋⢉⣉⣀⡤
⠀*  ⠰⡄⠹⣿⣿⣿⣿⠿⠛⠛⠛⠉⠉⠉⠀⠀⢠⣤⣤⣴⣶⣶⣿⣿⣿⣿⡿⠁
⠀*  ⠀⠘⣦⠈⢿⣿⣿⡇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⠙⠛⠿⣿⣿⣿⣿⡟⠀
⠀*  ⠀⠀⠘⢷⡄⠹⣿⡇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣼⣿⣿⡟⠀
⠀*  ⠀⠀⠀⠈⢿⣦⠈⢿⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣸⣿⣿⠏⠀
⠀*  ⠀⠀⠀⠀⠈⢻⣷⡄⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢠⣿⣿⠏⠀
⠀*  ⠀⠀⠀⠀⠀⠀⢻⣿⣦⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⣿⣿⠃
⠀*  ⠀⠀⠀⠀⠀⠀⠀⠻⣿⣷⡄⠀⠀⠀⢀⣠⡞⠀⣾⡿⠃
⠀*  ⠀⠀⠀⠀⠀⠀⠀⠀⠹⣿⣿⣦⣠⣴⣿⣿⠁⣸⡿⠁
⠀*  ⠀⠀⠀⠀⠀⠀⠀⠀⠀⠹⣿⣿⣿⣿⣿⠇⢰⡟⠁
⠀*  ⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠘⣿⣿⣿⡟⢀⡟
⠀*  ⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠘⣿⡿⠁⠎
⠀*  ⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⠃
 *
 *    Cry - Cryptor
 *    An ignifi Project © 2023
 *
 *    Licence : BSD 3-Clause License
 *    see file: LICENCE
 *
 *    Contact: cry.2023.gitlab.x9j3f22@ignifi.eu
 *
 */

#ifndef CRY_H
#define CRY_H

#pragma once

#include <cry/common.h>
#include <cry/config.h>

#include <stdint.h>
#include <string>

namespace cry {

struct CryptDetail {
  size_t keylength;
  std::vector<uint8_t> key;
  size_t blocklength;
  std::vector<uint8_t> block;
};

class Cry {
public:
  Cry();
  ~Cry() = default;

  // ==========================================

  void Encrypt(const std::string &file, EncryptionType type = gpg_algo) const;
  void Decrypt(const std::string &file, EncryptionType type = gpg_algo) const;
  void SetOutput(const std::string &output);
  void SetPassword(const std::string &password);

  // ==========================================

private:
  CryptDetail EncryptionSetup(gcry_cipher_hd_t &hd, EncryptionType type) const;

  std::vector<uint8_t> EncryptBlock(const std::vector<uint8_t> &bufferin,
                                    const size_t blockSize,
                                    gcry_cipher_hd_t &hd) const;

  std::vector<uint8_t> PadToBlock(const std::vector<uint8_t> &in,
                                  const size_t blocksize) const;

  void EncryptImpl(const std::string &file_in,
                   EncryptionType type = gpg_algo) const;
  void DecryptImpl(const std::string &file_in,
                   EncryptionType type = gpg_algo) const;

  size_t IdentifyFile(const std::string &file_in) const;

  CryptDetail GetCryptDetails(EncryptionType type) const;

  std::vector<uint8_t> CryptToLength(const std::vector<uint8_t> &in,
                                     size_t len) const;

  std::vector<uint8_t> RandomBytes(size_t sz) const;

  uint8_t ScaleUIntToUChar(uint32_t i) const;

  // ==========================================

  std::string _password;
  std::vector<uint8_t> _pwd;
  std::string _output;
};
} // namespace cry

#endif // CRY_H
