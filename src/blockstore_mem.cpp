#include "blockstore.h"
#include <unordered_map>
#include <list>
#include <string>
#include "utils.hpp"
#include <fstream>
#include <chrono>
#include <filesystem>

// Simple in-memory LRU cache BlockStore for testing
class MemBlockStore : public BlockStore
{
public:
    MemBlockStore(size_t capacity = 128) : capacity_(capacity) {}
    MemBlockStore(size_t capacity = 128, const std::string &evict_log = ".neuron/evict.log")
        : capacity_(capacity), evict_log_(evict_log)
    {
        // ensure directory exists for evict log
        auto p = std::filesystem::path(evict_log_).parent_path();
        if (!p.empty())
            std::filesystem::create_directories(p);
    }

    bool has(const std::string &hash) override
    {
        return map_.find(hash) != map_.end();
    }

    bool put(const std::string &hash, const std::string &data) override
    {
        auto it = map_.find(hash);
        if (it != map_.end())
        {
            touch(it);
            return true;
        }

        if (map_.size() >= capacity_)
        {
            // evict least-recently-used
            std::string ev = order_.back();
            // log eviction
            try
            {
                std::ofstream f(evict_log_, std::ios::app);
                if (f)
                {
                    auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
                    f << "{\"time\": " << now << ", \"evicted\": \"" << ev << "\", \"reason\": \"capacity\"}\n";
                }
            }
            catch (...)
            {
            }

            map_.erase(ev);
            order_.pop_back();
        }

        order_.push_front(hash);
        map_[hash] = {data, order_.begin()};
        return true;
    }

    std::string get(const std::string &hash) override
    {
        auto it = map_.find(hash);
        if (it == map_.end())
            return std::string();
        touch(it);
        return it->second.data;
    }

private:
    struct Entry
    {
        std::string data;
        std::list<std::string>::iterator it;
    };
    void touch(std::unordered_map<std::string, Entry>::iterator &it)
    {
        order_.splice(order_.begin(), order_, it->second.it);
        it->second.it = order_.begin();
    }

    size_t capacity_;
    std::list<std::string> order_;
    std::unordered_map<std::string, Entry> map_;
    std::string evict_log_;
};

// Factory helper for tests
BlockStore *make_mem_blockstore(size_t cap, const std::string &evict_log_path)
{
    return new MemBlockStore(cap, evict_log_path);
}

// Compatibility overload
BlockStore *make_mem_blockstore(size_t cap)
{
    return new MemBlockStore(cap, ".neuron/evict.log");
}
