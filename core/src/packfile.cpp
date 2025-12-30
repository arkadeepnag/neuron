#include "packfile.hpp"
#include "repository.hpp"
#include "sha1.hpp"
#include <cstring>

static void write_u32(std::vector<uint8_t>& out, uint32_t v) {
    out.push_back(v >> 24);
    out.push_back(v >> 16);
    out.push_back(v >> 8);
    out.push_back(v);
}

std::vector<uint8_t> create_pack(
    Repository& repo,
    const std::vector<std::string>& objects
) {
    std::vector<uint8_t> out;
    out.insert(out.end(), {'P','A','C','K'});
    write_u32(out, 1);
    write_u32(out, objects.size());

    for (auto& hash : objects) {
        auto obj = repo.read_object(hash);
        out.insert(out.end(), hash.begin(), hash.end());
        out.push_back('\0');
        out.insert(out.end(), obj.data.begin(), obj.data.end());
    }

    std::string hash = sha1_hex(std::string(out.begin(), out.end()));
    out.insert(out.end(), hash.begin(), hash.end());
    return out;
}

void apply_pack(
    Repository& repo,
    const std::vector<uint8_t>& pack
) {
    size_t i = 8;
    uint32_t count =
        (pack[4]<<24)|(pack[5]<<16)|(pack[6]<<8)|pack[7];

    for (uint32_t n = 0; n < count; ++n) {
        std::string hash;
        while (pack[i] != '\0') hash += pack[i++];
        i++;

        std::vector<uint8_t> obj;
        while (i < pack.size() && pack[i] != 'P')
            obj.push_back(pack[i++]);

        Object o;
        o.data = obj;
        repo.write_object(o);
    }
}
