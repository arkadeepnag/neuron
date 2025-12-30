#include "repository.hpp"
#include "sha1.hpp"

#include <filesystem>
#include <vector>
#include <stdexcept>
#include <cstring>

#include <fcntl.h>
#include <unistd.h>

namespace fs = std::filesystem;

Repository::Repository(const std::string& repo_path)
    : repo_root(repo_path)
{
}

/*
 Write object:
 - serialize with header
 - hash
 - write to temp file
 - fsync
 - atomic rename
*/
std::string Repository::write_object(const Object& obj) {
    // Serialize object
    std::string raw = serialize_object(obj);
    std::string hash = sha1_hex(raw);

    // Object path
    fs::path dir  = repo_root + "/objects/" + hash.substr(0, 2);
    fs::path file = dir / hash.substr(2);
    fs::path tmp  = file.string() + ".tmp";

    fs::create_directories(dir);

    // If object already exists, do nothing (content-addressed)
    if (fs::exists(file)) {
        return hash;
    }

    // POSIX write (NOT ofstream)
    int fd = ::open(tmp.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0444);
    if (fd < 0) {
        throw std::runtime_error("open() failed while writing object");
    }

    ssize_t total = 0;
    ssize_t size  = static_cast<ssize_t>(raw.size());
    const char* buf = raw.data();

    while (total < size) {
        ssize_t n = ::write(fd, buf + total, size - total);
        if (n <= 0) {
            ::close(fd);
            throw std::runtime_error("write() failed while writing object");
        }
        total += n;
    }

    if (::fsync(fd) != 0) {
        ::close(fd);
        throw std::runtime_error("fsync() failed while writing object");
    }

    ::close(fd);

    // Atomic replace
    fs::rename(tmp, file);

    return hash;
}

/*
 Read loose object:
 - open
 - read fully
*/
Object Repository::read_object(const std::string& hash) {
    fs::path file =
        repo_root + "/objects/" +
        hash.substr(0, 2) + "/" +
        hash.substr(2);

    int fd = ::open(file.c_str(), O_RDONLY);
    if (fd < 0) {
        throw std::runtime_error("object not found: " + hash);
    }

    std::vector<uint8_t> data;
    constexpr size_t BUF_SIZE = 8192;
    uint8_t buf[BUF_SIZE];

    while (true) {
        ssize_t n = ::read(fd, buf, BUF_SIZE);
        if (n < 0) {
            ::close(fd);
            throw std::runtime_error("read() failed");
        }
        if (n == 0) break;
        data.insert(data.end(), buf, buf + n);
    }

    ::close(fd);

    Object obj;
    obj.data = std::move(data);
    return obj;
}

const std::string& Repository::root() const {
    return repo_root;
}

