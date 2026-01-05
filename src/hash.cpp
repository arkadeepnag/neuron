#include "hash.hpp"
#include <openssl/sha.h>
#include <sstream>
#include <iomanip>

std::string sha256(const std::string &data)
{
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256((const unsigned char *)data.data(), data.size(), hash);

    std::ostringstream os;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++)
        os << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];

    return os.str();
}
