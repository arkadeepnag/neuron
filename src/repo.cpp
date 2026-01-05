#include "repo.h"
#include "utils.hpp"
#include <iostream>
#include <sstream>
#include "hash.hpp"
#include <zlib.h>
#include "refs.h"
#include "blockstore.h"

void repo_init()
{
    ensure_dir(".neuron/objects");
    ensure_dir(".neuron/refs/heads");

    write_file(".neuron/HEAD", "refs/heads/main\n");
    write_file(".neuron/refs/heads/main", "");

    write_file(".neuron/index", "");

    printf("Initialized empty Neuron repository\n");
}

static std::string read_object(const std::string &hash)
{
    // Use blockstore to fetch object (BlockStore returns raw uncompressed bytes)
    auto bs = get_default_blockstore();
    std::string full = bs->get(hash);
    return full;
}

void repo_log()
{
    std::string cur = read_file(".neuron/refs/heads/main");
    cur.erase(cur.find_last_not_of("\n") + 1);

    while (!cur.empty())
    {
        std::string obj = read_object(cur);
        std::cout << "commit " << cur << "\n";

        std::istringstream ss(obj);
        std::string line;
        std::string parent;

        while (std::getline(ss, line))
        {
            if (line.rfind("parent ", 0) == 0)
                parent = line.substr(7);
            if (line.empty())
                break;
        }

        std::cout << ss.str() << "\n";
        cur = parent;
    }
}

void create_branch(const std::string &name)
{
    std::string cur = get_head_commit();
    write_file(".neuron/refs/heads/" + name, cur);
}
