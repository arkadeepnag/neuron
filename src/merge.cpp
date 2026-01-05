#include "merge.h"
#include "refs.h"
#include "commit.h"
#include "utils.hpp"
#include "tree.h"
#include "object_store.hpp"
#include <sstream>
#include <ctime>

void merge_branch(const std::string &branch)
{
    std::string other = read_file(".neuron/refs/heads/" + branch);
    if (!other.empty())
        other.erase(other.find_last_not_of("\n") + 1);

    std::string current = get_head_commit();
    if (!current.empty())
        current.erase(current.find_last_not_of("\n") + 1);

    std::string message = "Merge branch " + branch;

    std::string tree_hash = write_tree();

    std::ostringstream commit;
    commit << "tree " << tree_hash << "\n";
    if (!current.empty())
        commit << "parent " << current << "\n";
    if (!other.empty())
        commit << "parent " << other << "\n";

    std::time_t t = std::time(nullptr);
    commit << "author Arkadeep <user@neuron> " << t << "\n\n";
    commit << message << "\n";

    std::string hash = store_object("commit", commit.str());
    update_head_commit(hash);
}
