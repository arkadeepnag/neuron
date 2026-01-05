#include "../src/blockstore.h"
#include <cassert>
#include <iostream>
#include <fstream>
#include <iterator>
#include <string>

int main()
{
    // create mem store with capacity 2
    BlockStore *m = make_mem_blockstore(2);

    m->put("a", "data-a");
    m->put("b", "data-b");
    // both present
    assert(m->has("a"));
    assert(m->has("b"));

    // add c -> should evict least recently used (a)
    m->put("c", "data-c");
    bool a_ok = m->has("a");
    bool b_ok = m->has("b");
    bool c_ok = m->has("c");

    if (a_ok)
    {
        std::cerr << "LRU eviction failed: 'a' still present\n";
        return 2;
    }
    if (!b_ok || !c_ok)
    {
        std::cerr << "LRU store missing expected entries\n";
        return 2;
    }

    std::cout << "MemBlockStore LRU eviction OK\n";

    // verify evict log contains 'a'
    std::ifstream f(".neuron/evict.log");
    if (!f)
    {
        std::cerr << "Evict log missing\n";
        return 3;
    }
    std::string content((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
    if (content.find("a") == std::string::npos)
    {
        std::cerr << "Evict log does not mention evicted key\n";
        return 4;
    }
    return 0;
}
