#include "index.hpp"
#include <fstream>

Index::Index(const std::string& path) : index_path(path + "/index") {
    std::ifstream in(index_path);
    std::string file, hash;
    while (in >> file >> hash)
        table[file] = hash;
}

void Index::add(const std::string& file, const std::string& hash) {
    table[file] = hash;
}

void Index::write() {
    std::ofstream out(index_path, std::ios::trunc);
    for (auto& [f, h] : table)
        out << f << " " << h << "\n";
}

std::map<std::string, std::string> Index::entries() const {
    return table;
}
