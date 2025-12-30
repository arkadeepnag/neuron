#include <iostream>
#include <string>
#include <vector>

#include "repository.hpp"
#include "negotiate.hpp"
#include "packfile.hpp"
#include "refs.hpp"    // ✅ REQUIRED

void handle_upload_pack(Repository& repo) {
    std::vector<std::string> haves;
    std::string line;

    while (std::getline(std::cin, line)) {
        if (line == "done") break;
        if (line.rfind("have ", 0) == 0)
            haves.push_back(line.substr(5));
    }

    std::string head = read_ref(repo.root(), "HEAD");
    std::vector<std::string> wants = { head };

    auto result = negotiate(repo, wants, haves);
    auto pack = create_pack(repo, result.send_objects);

    std::cout.write(
        reinterpret_cast<const char*>(pack.data()),
        pack.size()
    );
}

