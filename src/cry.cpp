
#include <cry/config.h>
#include <cry/cry.h>
extern "C" {
#include <gcrypt.h>
}
#include <math.h>

#include <boost/filesystem.hpp>
#include <boost/random.hpp>
#include <boost/random/random_device.hpp>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>

namespace fs = boost::filesystem;

namespace cry {
const std::string default_crypt_string = cry_default_iv;

//***************************************************************************************
//
//                                      CRY
//
//***************************************************************************************

Cry::Cry() {
  gcry_control(GCRYCTL_DISABLE_SECMEM, 0);
  gcry_control(GCRYCTL_INITIALIZATION_FINISHED, 0);
}

//===========================================================

void Cry::Encrypt(const std::string &file, EncryptionType type) const {
#if CRY_ENCRYPT
  EncryptImpl(file, type);
#endif
}

//===========================================================

void Cry::Decrypt(const std::string &file, EncryptionType type) const {
#if CRY_DECRYPT
  DecryptImpl(file, type);
#endif
}

//===========================================================

void Cry::SetOutput(const std::string &output) {
  _output = std::string{output};
}

//===========================================================

void Cry::SetPassword(const std::string &password) {
  _password = std::string{password};
  _pwd.clear();
  for (const auto &c : password) {
    _pwd.push_back(c);
  }
}

//===========================================================

void Cry::EncryptImpl(const std::string &file_in, EncryptionType type) const {
#if CRY_ENCRYPT
  auto file = std::string{file_in};
  gcry_cipher_hd_t hd;

  size_t filesize = IdentifyFile(file);

  if (filesize == 0) {
#if CRY_STDOUT
    std::cerr << "Could not find file" << std::endl;
#endif
    return;
  }

  CryptDetail cd = EncryptionSetup(hd, type);
  fs::path p(file);

  if (!fs::exists(p) && !fs::is_regular_file(p)) {
    return;
  }

  std::fstream fsin;
  fsin.open(p.string(), std::fstream::in | std::fstream::binary);
  std::stringstream ss;
  ss << file << cry_ext;
  std::fstream fsout;
  fsout.open(ss.str(), std::fstream::out | std::fstream::binary);
  CryHeader hdr;
  hdr.SetFileSize(filesize);
  hdr.SetFilename(file);
  auto h = hdr.WriteHeader();

  if (h.empty()) {
#if CRY_STDOUT
    std::cerr << "Unable to write header" << std::endl;
#endif
    return;
  }

  auto hp = PadToBlock(h, cd.blocklength);
  auto block = EncryptBlock(hp, cd.blocklength, hd);
  uint16_t blocks = block.size() / cd.blocklength;
  const char *magic = "#CRY3";
  fsout.write(magic, 5);
  uint16_t iv_sz = cd.block.size();
  fsout.write((char *)&iv_sz, sizeof(uint16_t));
  fsout.write((char *)cd.block.data(), cd.block.size());
  fsout.write(reinterpret_cast<char *>(&blocks), sizeof(uint16_t));
  fsout.write((char *)block.data(), block.size());

  if (filesize < cd.blocklength) {

    auto buff = new char[sizeof(char) * filesize];
    fsin.read(buff, filesize);
    std::vector<uint8_t> buffvec;
    buffvec.reserve(filesize);
    buffvec.assign(buff, buff + filesize);
    auto pad = PadToBlock(buffvec, cd.blocklength);
    auto enc = EncryptBlock(pad, cd.blocklength, hd);
    delete[] buff;
  } else {
    int64_t store = filesize;

    while (store > 0) {
      if (store < cd.blocklength) {
        auto buff = new char[sizeof(char) * store];
        fsin.read(buff, store);
        std::vector<uint8_t> buffvec;
        buffvec.reserve(store);
        buffvec.assign(buff, buff + store);
        auto pad = PadToBlock(buffvec, cd.blocklength);
        auto enc = EncryptBlock(pad, cd.blocklength, hd);
        fsout.write((char *)enc.data(), enc.size());
        delete[] buff;
      } else {
        auto buff = new char[sizeof(char) * cd.blocklength];
        fsin.read(buff, cd.blocklength);
        std::vector<uint8_t> buffvec;
        buffvec.reserve(cd.blocklength);
        buffvec.assign(buff, buff + cd.blocklength);
        auto enc = EncryptBlock(buffvec, cd.blocklength, hd);
        fsout.write((char *)enc.data(), enc.size());
        delete[] buff;
      }

      store -= cd.blocklength;
    }
  }

  fsout.close();
  fsin.close();
  gcry_cipher_close(hd);
#endif // CRY_ENCRYPT
}

void Cry::DecryptImpl(const std::string &file_in, EncryptionType type) const {
#if CRY_DECRYPT
  auto file = std::string{file_in};
  gcry_cipher_hd_t hd;
  auto cd = EncryptionSetup(hd, type);
  fs::path p(file);

  if (!fs::exists(p) && !fs::is_regular_file(p)) {
    return;
  }

  std::fstream fs;
  fs.open(p.string(), std::fstream::in | std::fstream::binary);
  std::vector<uint8_t> cryptbuffer;

  size_t filesz = fs::file_size(file);

  if (filesz == 0) {
#if CRY_STDOUT
    std::cerr << "Cry file empty" << std::endl;
#endif
    return;
  }

  char magic[6];
  fs.read(magic, 5);
  magic[5] = '\0';

  if (strncmp("#CRY3", magic, 5) != 0) {
#if CRY_STDOUT
    std::cerr << "Not a cry file" << std::endl;
#endif
    return;
  }

  uint16_t iv_sz = 0;
  fs.read((char *)&iv_sz, sizeof(uint16_t));

  if (iv_sz == 0) {
    return;
  }

  uint8_t *iv = new uint8_t[iv_sz];
  fs.read((char *)iv, iv_sz);

  cd.block.clear();

  for (size_t iv_i = 0; iv_i < iv_sz; ++iv_i) {
    cd.block.push_back(iv[iv_i]);
  }

  delete[] iv;

  gcry_cipher_setiv(hd, cd.block.data(), cd.block.size());

  uint16_t b = 0;
  fs.read(reinterpret_cast<char *>(&b), sizeof(uint16_t));
  size_t bsz = cd.blocklength * b;

  auto buffinmain = new char[sizeof(char) * bsz];
  fs.read(buffinmain, bsz);

  auto buffdecmain = new char[sizeof(char) * bsz];
  gcry_error_t err = gcry_cipher_decrypt(hd, buffdecmain, bsz, buffinmain, bsz);

  delete[] buffinmain;
  buffinmain = nullptr;
  cryptbuffer.reserve(bsz);
  cryptbuffer.assign(buffdecmain, buffdecmain + (bsz));
  delete[] buffdecmain;

  if (err > 0) {
    auto e = gcry_err_code(err);
#if CRY_STDOUT
    std::cerr << "Could not decrypt - error code: " << e << std::endl;
#endif
  }

  CryHeader ch;
  size_t sz = ch.ReadHeader(cryptbuffer);

  if (sz == 0) {
#if CRY_STDOUT
    std::cerr << "Could not read header" << std::endl;
#endif
    return;
  }

  std::stringstream ssout;
  // Filename of output file
  ssout << std::string(ch.Filename(), ch.FilenameSize());

  if (fs::exists(ssout.str())) {
    std::cout << "File: \"" << ssout.str() << " already exists!" << std::endl;
    std::cout << "Do you want to overwrite? (Y/N)" << std::endl;
    std::string input;
    std::cin >> input;

    if (input == "Y" || input == "y") {
      // do nothing
    } else {
      ssout << ".out";
    }
  }

  std::fstream fsout;

  if (_output.empty()) {
    fsout.open(ssout.str(), std::fstream::out | std::fstream::binary);
  } else {
    fsout.open(_output, std::fstream::out | std::fstream::binary);
  }

  if (fsout.fail() || fsout.bad()) {
#if CRY_STDOUT
    std::cerr << "Could not open: " << ssout.str() << std::endl;
#endif
  }

  size_t filesizecount = ch.FileSize();
  float div = float(filesizecount) / float(cd.blocklength);
  size_t blocks = ceil(div);

  while (blocks > 0) {
    if (filesizecount < cd.blocklength) {

      auto buffdec = new char[sizeof(char) * cd.blocklength];
      auto buffin = new char[sizeof(char) * cd.blocklength];
      fs.read(buffin, cd.blocklength);
      err = gcry_cipher_decrypt(hd, buffdec, cd.blocklength, buffin,
                                cd.blocklength);

      if (err > 0) {
#if CRY_STDOUT
        auto ec = gcry_err_code(err);
        std::cerr << "Error while decrypting: " << ec << std::endl;
        delete[] buffin;
        delete[] buffdec;
        return;
#endif
      } else {
        fsout.write(buffdec, filesizecount);
        delete[] buffin;
        delete[] buffdec;
      }
    } else {

      auto buffdec = new char[sizeof(char) * cd.blocklength];
      auto buffin = new char[sizeof(char) * cd.blocklength];
      fs.read(buffin, cd.blocklength);
      err = gcry_cipher_decrypt(hd, buffdec, cd.blocklength, buffin,
                                cd.blocklength);

      if (err > 0) {
#if CRY_STDOUT
        auto ec = gcry_err_code(err);
        std::cerr << "Error while decrypting: " << ec << std::endl;
        delete[] buffin;
        delete[] buffdec;
#endif
      } else {
        fsout.write(buffdec, cd.blocklength);
        delete[] buffin;
        delete[] buffdec;
      }
    }

    --blocks;
    filesizecount -= cd.blocklength;
  }

  fsout.close();
  fs.close();
  gcry_cipher_close(hd);
#endif // CRY_DECRYPT
}

size_t Cry::IdentifyFile(const std::string &file_in) const {
  auto file = std::string{file_in};
  fs::path p(file);
  size_t filesize = 0;

  if (!fs::exists(p)) {
    return 0;
  }

  filesize = fs::file_size(p);
  return filesize;
}

CryptDetail Cry::GetCryptDetails(EncryptionType type) const {
  CryptDetail c;
  c.keylength = gcry_cipher_get_algo_keylen(type);

  c.key = CryptToLength(_pwd, c.keylength);
  c.blocklength = gcry_cipher_get_algo_blklen(type);
  auto vec = randomBytes(c.blocklength);
  std::stringstream ss;

  c.block = vec; // CryptToLength(cry_default_iv, c.blocklength);
  return c;
}

std::vector<uint8_t> Cry::CryptToLength(const std::vector<uint8_t> &in,
                                        size_t len) const {
  size_t l = in.size();

  if (l == 0) {
#if CRY_STDOUT
    std::cerr << "CryptToLength value to be expanded can't be empty"
              << std::endl;
#endif
    exit(1);
  }

  if (l == len) {
    return in;
  } else if (l > len) {
    return std::vector<uint8_t>(in.begin(), in.begin() + len);
  } else {
    std::vector<uint8_t> out;
    out.assign(in.begin(), in.end());
    size_t i = 0;
    size_t clen = in.size();

    while (out.size() < len) {
      auto b = in.at(i);
      out.push_back(b);
      ++i;

      if (i >= clen) {
        i = 0;
      }
    }

    return out;
  }
}

CryptDetail Cry::EncryptionSetup(gcry_cipher_hd_t &hd,
                                 EncryptionType type) const {

  gcry_error_t err = gcry_cipher_open(&hd, type, GCRY_CIPHER_MODE_CBC, 0);

  if (err != GPG_ERR_NO_ERROR) {
#if CRY_STDOUT
    std::cerr << "Couldn't open cipher: " << std::endl;
    std::cerr << "Code: " << err << std::endl;
#endif
    exit(1);
  }

  CryptDetail cd = GetCryptDetails(type);
  err = gcry_cipher_setkey(hd, cd.key.data(), cd.keylength);

  if (err != GPG_ERR_NO_ERROR) {
#if CRY_STDOUT
    std::cerr << "Couldn't set encryption key: " << std::endl;
    std::cerr << "Code: " << err << std::endl;
#endif
    exit(1);
  }

  err = gcry_cipher_setiv(hd, cd.block.data(), cd.blocklength);

  if (err != GPG_ERR_NO_ERROR) {
#if CRY_STDOUT
    std::cerr << "Couldn't set IV: " << std::endl;
    std::cerr << "Code: " << err << std::endl;
#endif
    exit(1);
  }

  return cd;
}

std::vector<uint8_t> Cry::EncryptBlock(const std::vector<uint8_t> &bufferin,
                                       const size_t blockSize,
                                       gcry_cipher_hd_t &hd) const {
  std::vector<uint8_t> outvec;
  size_t sz = bufferin.size();
  auto out = new uint8_t[sz];
  gcry_error_t err = gcry_cipher_encrypt(hd, out, sz, bufferin.data(), sz);

  if (err) {
#if CRY_STDOUT
    auto ec = gcry_err_code(err);
    std::cerr << "Gcrypt Error: " << ec << std::endl;
#endif
    return outvec;
  }

  // outvec.assign(out, out + sz);
  for (size_t i = 0; i < sz; ++i) {
    outvec.push_back(out[i]);
  }
  delete[] out;
  return outvec;
}

std::vector<uint8_t> Cry::randomBytes(size_t sz) const {
  std::vector<uint8_t> outvec;
  boost::random::random_device rdev;

  if (sz == 0) {
    return outvec;
  }

  for (size_t i = 0; i < sz; ++i) {
    auto val = rdev();
    uint8_t c = ScaleUIntToUChar(val);
    outvec.push_back(c);
  }
  return outvec;
}

std::vector<uint8_t> Cry::PadToBlock(const std::vector<uint8_t> &in,
                                     const size_t blocksize) const {
  std::vector<uint8_t> outvec;
  boost::random::random_device rdev;

  if (in.size() <= blocksize) {
    outvec.reserve(blocksize);
    outvec.assign(in.begin(), in.end());

    while (outvec.size() < blocksize) {
      auto val = rdev();
      uint8_t c = ScaleUIntToUChar(val);
      outvec.push_back(c);
    }
  } else {
    float div = float(in.size()) / float(blocksize);
    int count = ceil(div);
    size_t sz = blocksize * count;
    outvec.reserve(sz);
    outvec.assign(in.begin(), in.end());

    while (outvec.size() < sz) {
      auto val = rdev();
      uint8_t c = ScaleUIntToUChar(val);
      outvec.push_back(c);
    }
  }

  return outvec;
}

uint8_t Cry::ScaleUIntToUChar(uint32_t i) const {
  double div = (double)i / (double)UINT32_MAX;
  double m = div * UINT8_MAX;
  auto c = (uint8_t)m;
  return c;
}

} // namespace cry
