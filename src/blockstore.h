#pragma once
#include <string>
#include <vector>

class BlockStore
{
public:
    virtual ~BlockStore() = default;
    // Return true if block exists
    virtual bool has(const std::string &hash) = 0;
    // Put raw bytes for block (implementation may compress or transform)
    virtual bool put(const std::string &hash, const std::string &data) = 0;
    // Get raw bytes, empty if not found
    virtual std::string get(const std::string &hash) = 0;
};

// Access the global default blockstore (filesystem-backed by default)
BlockStore *get_default_blockstore();
void set_default_blockstore(BlockStore *bs);

// Create an in-memory LRU BlockStore (tests/clients can take ownership)
BlockStore *make_mem_blockstore(size_t capacity, const std::string &evict_log_path = ".neuron/evict.log");

// Create a chained blockstore: queries backends in order, writes to all backends on put
BlockStore *make_chained_blockstore(const std::vector<BlockStore *> &backends);
