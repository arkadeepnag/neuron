#include "pack_index.hpp"
#include <fstream>

PackIndex::PackIndex(const std::string& path) {
    std::ifstream in(path, std::ios::binary);
    std::string hash;
    size_t off;
    while (in >> hash >> off)
        table[hash] = off;
}

bool PackIndex::has(const std::string& h) const {
    return table.count(h);
}

size_t PackIndex::offset(const std::string& h) const {
    return table.at(h);
}
