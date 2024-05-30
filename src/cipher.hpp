//
// Created by ian on 5/30/24.
//

#ifndef CIPHER_HPP
#define CIPHER_HPP

#include <iostream>
#include <openssl/conf.h>
#include <openssl/evp.h>
#include <openssl/err.h>
#include <openssl/pem.h>

#include <boost/algorithm/hex.hpp>


namespace cipher {
    inline std::mutex lock;

    inline EVP_PKEY * get_private_rsa() {
        std::string _path = "certificates/authority.pem";
        FILE* _public_key_file = fopen(_path.c_str(), "rb");

        if (!_public_key_file)
            throw std::runtime_error("certificates/authority.pem should exists as file.");

        EVP_PKEY* _private_key = PEM_read_PrivateKey(_public_key_file, nullptr, nullptr, nullptr);

        if (!_private_key)
            throw std::runtime_error("certificates/authority.pem should be a valid private key.");

        fclose(_public_key_file);
        return _private_key;
    }

    inline std::string decrypt(const std::string & input) {
        EVP_PKEY * _private_key = get_private_rsa();
        EVP_PKEY_CTX * _context = EVP_PKEY_CTX_new(_private_key, nullptr);
        std::string _input = boost::algorithm::unhex(input);

        if (!_context)
            throw std::runtime_error("OpenSSL can't create context.");

        if (EVP_PKEY_decrypt_init(_context) <= 0)
            throw std::runtime_error("OpenSSL can't init the decryption.");

        size_t _cipher_text_length = 0;

        if (EVP_PKEY_decrypt(_context, nullptr, &_cipher_text_length, (const unsigned char *)_input.c_str(), _input.length()) <= 0)
            throw std::runtime_error("OpenSSL can't decrypt.");

        std::string _cipher_text(_cipher_text_length, '\0');

        if (EVP_PKEY_decrypt(_context, (unsigned char *) _cipher_text.data(), &_cipher_text_length, (const unsigned char *)_input.c_str(), _input.length()) <= 0)
            throw std::runtime_error("OpenSSL can't decrypt.");

        std::lock_guard scoped_lock(lock);
        EVP_PKEY_CTX_free(_context);

        _cipher_text.resize(_cipher_text_length);

        return _cipher_text;
    }
}



#endif //CIPHER_HPP
