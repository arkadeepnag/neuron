#include <iostream>
#include <vector>

#include "repository.hpp"
#include "packfile.hpp"
#include "refs.hpp"

void handle_receive_pack(Repository& repo) {
    std::vector<uint8_t> pack(
        (std::istreambuf_iterator<char>(std::cin)),
        std::istreambuf_iterator<char>()
    );

    apply_pack(repo, pack);

}

