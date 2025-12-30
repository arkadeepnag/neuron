#include "mmap.hpp"
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdexcept>

MMap mmap_file(const std::string& path) {
    int fd = open(path.c_str(), O_RDONLY);
    if (fd < 0) throw std::runtime_error("open failed");

    size_t size = lseek(fd, 0, SEEK_END);
    void* data = mmap(nullptr, size, PROT_READ, MAP_PRIVATE, fd, 0);
    close(fd);

    if (data == MAP_FAILED)
        throw std::runtime_error("mmap failed");

    return {data, size};
}

void munmap_file(MMap& m) {
    munmap(m.data, m.size);
}
