#include "../src/blockstore.h"
#include <cassert>
#include <iostream>

int main()
{
    // Use filesystem-backed default
    BlockStore *bs = get_default_blockstore();

    std::string hash = "deadbeef"; // fake hash for test
    std::string data = "hello world";

    // ensure not present
    if (bs->has(hash))
        std::cerr << "Warning: test hash already present\n";

    bool ok = bs->put(hash, data);
    assert(ok);
    assert(bs->has(hash));
    std::string out = bs->get(hash);
    assert(out == data);

    std::cout << "BlockStore FS basic put/get OK\n";
    return 0;
}
