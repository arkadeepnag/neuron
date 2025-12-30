#include "repository.hpp"
#include "packfile.hpp"
#include "refs.hpp"
#include "object.hpp"

#include <iostream>
#include <sstream>
#include <vector>
#include <stdexcept>

static bool is_fast_forward(
    Repository& repo,
    const std::string& old_head,
    const std::string& new_head
) {
    std::string cur = new_head;

    while (!cur.empty()) {
        if (cur == old_head) return true;

        Object obj;
        try {
            obj = repo.read_object(cur);
        } catch (...) {
            // Object missing or unreadable
            return false;
        }

        if (obj.type != ObjectType::Commit) {
            return false;
        }

        if (obj.data.empty()) {
            return false;
        }

        std::istringstream in(
            std::string(obj.data.begin(), obj.data.end())
        );

        std::string line;
        std::string parent;

        while (std::getline(in, line)) {
            if (line.rfind("parent ", 0) == 0) {
                parent = line.substr(7);
                break;
            }
            if (line.empty()) break;
        }

        if (parent.empty()) {
            // Root commit reached
            return false;
        }

        cur = parent;
    }

    return false;
}

void handle_receive_pack(Repository& repo) {
    // 1) Read ref update line
    std::string line;
    if (!std::getline(std::cin, line)) {
        throw std::runtime_error("missing ref update line");
    }

    std::istringstream refline(line);
    std::string old_head, new_head, refname;
    refline >> old_head >> new_head >> refname;

    if (refname.empty()) {
        throw std::runtime_error("invalid ref update");
    }

    // 2) Read pack stream
    std::vector<uint8_t> pack;
    uint8_t buf[4096];

    while (std::cin.read(reinterpret_cast<char*>(buf), sizeof(buf))) {
        pack.insert(pack.end(), buf, buf + sizeof(buf));
    }
    pack.insert(pack.end(), buf, buf + std::cin.gcount());

    // 3) Apply pack
    if (!pack.empty()) {
        apply_pack(repo, pack);
    }

    const std::string root = repo.root();
    std::string current = read_ref(root, refname);

    // 4) Validate ref state
    if (!current.empty() && current != old_head) {
        throw std::runtime_error("ref mismatch");
    }

    // 5) Fast-forward check
    if (!current.empty()) {
        if (!is_fast_forward(repo, current, new_head)) {
            throw std::runtime_error("non-fast-forward push rejected");
        }
    }

    // 6) Update refs
    update_ref(root, refname, new_head);
    update_ref(root, "HEAD", new_head);

    std::cout << "ok\n";
}

