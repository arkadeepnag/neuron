#include "checkout.h"
#include "utils.hpp"
#include <sstream>
#include <zlib.h>

static std::string read_object(const std::string &hash)
{
    std::string path = ".neuron/objects/" + hash.substr(0, 2) + "/" + hash.substr(2);
    std::string compressed = read_file(path);

    std::string out(8192, '\0');
    uLongf size = out.size();
    uncompress((Bytef *)out.data(), &size,
               (Bytef *)compressed.data(), compressed.size());
    out.resize(size);
    return out;
}

void checkout_commit(const std::string &commit_hash)
{
    std::string commit = read_object(commit_hash);

    std::istringstream ss(commit);
    std::string line, tree_hash;

    while (std::getline(ss, line))
    {
        if (line.rfind("tree ", 0) == 0)
        {
            tree_hash = line.substr(5);
            break;
        }
    }

    std::string tree = read_object(tree_hash);
    std::istringstream ts(tree);

    while (std::getline(ts, line, '\0'))
    {
        if (line.empty())
            continue;

        std::istringstream ls(line);
        std::string mode, path, blob_hash;
        ls >> mode >> path;
        ts >> blob_hash;

        std::string blob = read_object(blob_hash);
        auto pos = blob.find('\0');
        write_file(path, blob.substr(pos + 1));
    }
}
