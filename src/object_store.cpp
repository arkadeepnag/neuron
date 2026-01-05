#include "object_store.hpp"
#include "hash.hpp"
#include "utils.hpp"
#include <zlib.h>

std::string store_object(
    const std::string &type,
    const std::string &content,
    BlockStore *bs)
{
    if (!bs)
        bs = get_default_blockstore();

    std::string header = type + " " + std::to_string(content.size()) + '\0';
    std::string full = header + content;

    std::string hash = sha256(full);

    if (bs->has(hash))
        return hash;

    // Delegate storage (including any compression) to BlockStore implementation
    bs->put(hash, full);
    return hash;
}
