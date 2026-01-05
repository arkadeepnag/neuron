#include "tree.h"
#include "utils.hpp"
#include "object_store.hpp"
#include <sstream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <map>

struct Entry
{
    std::string path;
    std::string hash;
};

// Recursively write a tree for the given set of entries where each entry.path
// is relative to the current directory (no leading '/'). Returns the tree hash.
static std::string write_tree_recursive(const std::vector<Entry> &entries)
{
    // Separate files and directories at this level
    std::vector<Entry> files;
    std::map<std::string, std::vector<Entry>> subdirs;

    for (auto &e : entries)
    {
        auto pos = e.path.find('/');
        if (pos == std::string::npos)
        {
            files.push_back(e);
        }
        else
        {
            std::string dir = e.path.substr(0, pos);
            std::string rest = e.path.substr(pos + 1);
            subdirs[dir].push_back({rest, e.hash});
        }
    }

    // Build tree content: first files, then subtrees
    std::ostringstream tree;

    // Files
    std::sort(files.begin(), files.end(), [](auto &a, auto &b)
              { return a.path < b.path; });
    for (auto &f : files)
    {
        tree << "100644 " << f.path << '\0';
        tree << f.hash;
    }

    // Subdirectories
    for (auto &kv : subdirs)
    {
        const std::string &dirname = kv.first;
        std::vector<Entry> child_entries = kv.second;
        // Recursively write child tree
        std::string child_tree_hash = write_tree_recursive(child_entries);
        tree << "40000 " << dirname << '\0';
        tree << child_tree_hash;
    }

    return store_object("tree", tree.str());
}

std::string write_tree()
{
    std::ifstream idx(".neuron/index");
    std::vector<Entry> entries;

    std::string hash, path;
    while (idx >> hash >> path)
    {
        entries.push_back({path, hash});
    }

    if (entries.empty())
        return std::string();

    return write_tree_recursive(entries);
}
