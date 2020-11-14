#include "encoder.h"
#include "base64/base64.h"
#include "cryptopp/cryptlib.h"
#include "cryptopp/rijndael.h"
#include "cryptopp/aes.h"
#include "cryptopp/modes.h"
#include "cryptopp/filters.h"
#include <algorithm>

namespace cppbackend {
    std::string Encoder::toBase64(const std::string &s)
    {
        return base64_encode(s);
    }

    std::string Encoder::toAES128(const std::string &s, const std::string& password)
    {
        if (password.length() != PASSWORD_LENGTH_128)
        {
            throw std::invalid_argument("Password does not meet length requirement");
        }

        CryptoPP::byte key[PASSWORD_LENGTH_128];
        password.copy(reinterpret_cast<char*>(key), PASSWORD_LENGTH_128);

        CryptoPP::byte iv[CryptoPP::AES::BLOCKSIZE];
        std::fill(std::begin(iv), std::end(iv), 0x00);

        CryptoPP::AES::Encryption aesEncryption(key, PASSWORD_LENGTH_128);
        CryptoPP::CBC_Mode_ExternalCipher::Encryption cbcEncryption(aesEncryption, iv);

        std::string cipherText;

        CryptoPP::StreamTransformationFilter encryptor(cbcEncryption, new CryptoPP::StringSink(cipherText));
        encryptor.Put(reinterpret_cast<const unsigned char*>(s.c_str()), s.length());
        encryptor.MessageEnd();

        return toBase64(cipherText);
    }
}
