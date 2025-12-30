#pragma once
#include <string>
#include <openssl/sha.h>

inline std::string sha1_hex(const std::string& data) {
    unsigned char hash[SHA_DIGEST_LENGTH];
    SHA1(reinterpret_cast<const unsigned char*>(data.data()), data.size(), hash);

    char out[41];
    for (int i = 0; i < 20; ++i)
        sprintf(out + i * 2, "%02x", hash[i]);
    out[40] = '\0';
    return std::string(out);
}
