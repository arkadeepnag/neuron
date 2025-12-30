#include "tree.hpp"
#include "object.hpp"
#include "repository.hpp"
#include <sstream>

std::string build_tree(
    const std::map<std::string, std::string>& index,
    Repository& repo
) {
    std::stringstream ss;
    for (auto& [path, hash] : index) {
        ss << "100644 " << path << '\0' << hash;
    }

    Object tree;
    tree.type = ObjectType::Tree;
    std::string s = ss.str();
    tree.data.assign(s.begin(), s.end());

    return repo.write_object(tree);
}
