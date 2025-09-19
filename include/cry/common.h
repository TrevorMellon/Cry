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

#ifndef CRY_COMMON_H
#define CRY_COMMON_H

#pragma once

#include <algorithm>
#include <cry/config.h>
#include <gcrypt.h>
#include <iostream>
#include <sstream>
#include <stdint.h>
#include <vector>

namespace cry {

using EncryptionType = gcry_cipher_algos;

class CryHeader {
public:
  // ==============================================

  CryHeader() = default;

  // ==============================================

  ~CryHeader() = default;

  // ==============================================

  size_t FileSize() const { return _filesize; }

  // ==============================================

  size_t FilenameSize() const { return _filename.size(); }

  // ==============================================

  const char *Filename() const { return _filename.data(); }

  // ==============================================

  size_t Size() const {
    size_t ret = 0;
    ret += _filename.size();
    ret += 2 * sizeof(uint64_t);
    ret += 2;
    return ret;
  }

  // ==============================================

  void Clear() {
    _filename.clear();
    _filesize = 0;
  }

  // ==============================================

  void SetFileSize(const size_t fs) { _filesize = fs; }

  // ==============================================

  void SetFilename(const std::string &fn) { _filename = fn; }

  // ==============================================

  size_t ReadHeader(const std::vector<uint8_t> &buffer) {
    
    const uint64_t *ptr64 = reinterpret_cast<const uint64_t*>(buffer.data());
    SetFileSize(ptr64[0]);
    auto fns = ptr64[1];

    if (fns >= FILENAME_MAX) {
#if CRY_STDOUT
      std::cerr << "Filename size too large" << std::endl;
#endif
      this->Clear();
      return 0;
    }

    std::stringstream filenamess;
    const auto *ptr = buffer.data();
    ptr += 2 * sizeof(uint64_t);

    if (*ptr != '\n') {
      return 0;
    }

    ++ptr;

    for (size_t i = 0; i < fns; ++i) {
      filenamess << (char)ptr[i];
    }

    std::string s = filenamess.str();

    if (s.size() != fns) {
#if CRY_STDOUT
      std::cerr << "Not a Cry encrypted file" << std::endl;
#endif
      return 0;
    }

    this->SetFilename(s);
    const auto headersz = this->Size();
    return headersz;
  }

  // ==============================================

  std::vector<uint8_t> WriteHeader() {
    std::vector<uint8_t> vec;
    vec.reserve(Size());
    uint64_t tmp;
    tmp = _filesize;
    
    uint8_t *ptr = reinterpret_cast<uint8_t*>(&tmp); 

    auto xz = sizeof(uint64_t);

    for (size_t i = 0; i < xz; ++i) {
      vec.push_back(ptr[i]);
    }

    tmp = _filename.size();
    
    ptr = reinterpret_cast<uint8_t*>(&tmp);

    for (size_t i =0; i < xz; ++i) {
      vec.push_back(ptr[i]);
    }


    vec.push_back('\n');

    std::for_each(_filename.begin(), _filename.end(), [&vec](const char &c) {
      auto cs = (uint8_t)c;
      vec.push_back(cs);
    });


    return vec;
  }

  // ==============================================

private:
  size_t _filesize = 0;
  std::string _filename;
};

} // namespace cry
#endif // CRY_COMMON_H
