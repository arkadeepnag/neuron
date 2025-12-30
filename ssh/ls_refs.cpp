#include <iostream>
#include "repository.hpp"
#include "refs.hpp"

void handle_ls_refs(Repository& repo) {
    auto refs = list_refs(repo.root());

    for (auto& [name, hash] : refs) {
        if (!hash.empty()) {
            std::cout << name << " " << hash << "\n";
        }
    }
}
