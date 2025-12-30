#pragma once
#include <string>
#include <unordered_map>

class PackIndex {
public:
    explicit PackIndex(const std::string& idx_path);
    bool has(const std::string& hash) const;
    size_t offset(const std::string& hash) const;

private:
    std::unordered_map<std::string, size_t> table;
};
