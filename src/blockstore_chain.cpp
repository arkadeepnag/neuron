#include "blockstore.h"
#include <vector>
#include <algorithm>

class ChainedBlockStore : public BlockStore
{
public:
    ChainedBlockStore(const std::vector<BlockStore *> &bs) : backends(bs) {}

    bool has(const std::string &hash) override
    {
        for (auto b : backends)
            if (b->has(hash))
                return true;
        return false;
    }

    bool put(const std::string &hash, const std::string &data) override
    {
        bool ok = true;
        for (auto b : backends)
        {
            ok = b->put(hash, data) && ok;
        }
        return ok;
    }

    std::string get(const std::string &hash) override
    {
        for (size_t i = 0; i < backends.size(); ++i)
        {
            auto b = backends[i];
            if (b->has(hash))
            {
                std::string data = b->get(hash);
                // warm earlier caches
                for (size_t j = 0; j < i; ++j)
                    backends[j]->put(hash, data);
                return data;
            }
        }
        return std::string();
    }

private:
    std::vector<BlockStore *> backends;
};

BlockStore *make_chained_blockstore(const std::vector<BlockStore *> &backends)
{
    return new ChainedBlockStore(backends);
}
