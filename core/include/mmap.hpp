#pragma once
#include <cstddef>
#include <string>

struct MMap {
    void* data;
    size_t size;
};

MMap mmap_file(const std::string& path);
void munmap_file(MMap& m);
