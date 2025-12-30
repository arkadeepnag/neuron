#pragma once
#include <map>
#include <string>

class Index {
public:
    explicit Index(const std::string& path);

    void add(const std::string& file, const std::string& hash);
    std::map<std::string, std::string> entries() const;
    void write();

private:
    std::string index_path;
    std::map<std::string, std::string> table;
};
