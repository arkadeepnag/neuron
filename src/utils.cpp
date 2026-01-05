#include "utils.hpp"
#include <filesystem>
#include <fstream>

bool file_exists(const std::string &path)
{
    return std::filesystem::exists(path);
}

void ensure_dir(const std::string &path)
{
    std::filesystem::create_directories(path);
}

std::string read_file(const std::string &path)
{
    std::ifstream f(path, std::ios::binary);
    return std::string(
        (std::istreambuf_iterator<char>(f)),
        std::istreambuf_iterator<char>());
}

void write_file(const std::string &path, const std::string &data)
{
    std::ofstream f(path, std::ios::binary);
    f.write(data.data(), data.size());
}
