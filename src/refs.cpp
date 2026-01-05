#include "refs.h"
#include "utils.hpp"

std::string get_head_ref()
{
    std::string head = read_file(".neuron/HEAD");
    head.erase(head.find_last_not_of("\n") + 1);
    return head;
}

std::string get_head_commit()
{
    std::string ref = get_head_ref();
    return read_file(".neuron/" + ref);
}

void update_head_commit(const std::string &hash)
{
    std::string ref = get_head_ref();
    write_file(".neuron/" + ref, hash + "\n");
}
