#pragma once
#include <string>

bool file_exists(const std::string &path);
void ensure_dir(const std::string &path);
std::string read_file(const std::string &path);
void write_file(const std::string &path, const std::string &data);
