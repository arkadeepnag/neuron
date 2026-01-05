#include "index.hpp"
#include "utils.hpp"
#include "object_store.hpp"
#include <fstream>

void index_add(const std::string &path)
{
    std::string content = read_file(path);
    std::string hash = store_object("blob", content);

    std::ofstream idx(".neuron/index", std::ios::app);
    idx << hash << " " << path << "\n";
}
