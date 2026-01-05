#include "blockstore.h"
#include "utils.hpp"
#include <filesystem>
#include <zlib.h>

class FSBlockStore : public BlockStore
{
public:
    bool has(const std::string &hash) override
    {
        std::string file = path_for(hash);
        return std::filesystem::exists(file);
    }

    bool put(const std::string &hash, const std::string &data) override
    {
        std::string file = path_for(hash);
        if (std::filesystem::exists(file))
            return true;

        std::filesystem::create_directories(std::filesystem::path(file).parent_path());

        // compress before writing
        uLongf compressed_size = compressBound(data.size());
        std::string compressed(compressed_size, '\0');
        compress((Bytef *)compressed.data(), &compressed_size, (Bytef *)data.data(), data.size());
        compressed.resize(compressed_size);

        write_file(file, compressed);
        return true;
    }

    std::string get(const std::string &hash) override
    {
        std::string file = path_for(hash);
        if (!std::filesystem::exists(file))
            return std::string();
        std::string compressed = read_file(file);

        // decompress
        std::string out(4096, '\0');
        uLongf size = out.size();
        int res = uncompress((Bytef *)out.data(), &size, (Bytef *)compressed.data(), compressed.size());
        if (res != Z_OK)
            return std::string();
        out.resize(size);
        return out;
    }

private:
    static std::string path_for(const std::string &hash)
    {
        return std::string(".neuron/objects/") + hash.substr(0, 2) + "/" + hash.substr(2);
    }
};

static FSBlockStore default_fs_store;
static BlockStore *global_blockstore = nullptr;

// Initialize default blockstore as an LRU memory cache layered over FS
__attribute__((constructor)) static void init_default_blockstore()
{
    // cache size from env, default 256
    size_t cap = 256;
    if (const char *env = std::getenv("NEURON_CACHE_SIZE"))
    {
        try
        {
            cap = std::stoul(env);
            if (cap == 0)
                cap = 1;
        }
        catch (...)
        {
            cap = 256;
        }
    }

    const char *logpath = std::getenv("NEURON_EVICT_LOG");
    std::string evict_log = logpath ? std::string(logpath) : std::string(".neuron/evict.log");

    BlockStore *mem = make_mem_blockstore(cap, evict_log);
    std::vector<BlockStore *> b = {mem, &default_fs_store};
    global_blockstore = make_chained_blockstore(b);
}

BlockStore *get_default_blockstore()
{
    if (!global_blockstore)
        global_blockstore = &default_fs_store;
    return global_blockstore;
}

void set_default_blockstore(BlockStore *bs)
{
    if (bs)
        global_blockstore = bs;
}
