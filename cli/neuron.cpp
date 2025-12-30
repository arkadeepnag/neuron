#include <iostream>
#include <fstream>
#include <vector>

#include "repository.hpp"
#include "index.hpp"
#include "tree.hpp"
#include "commit.hpp"
#include "refs.hpp"

int neuron_push(const std::string& url);
int neuron_pull(const std::string& url);

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "usage: neuron <command>\n";
        return 1;
    }

    std::string cmd = argv[1];

    if (cmd == "add") {
        if (argc < 3) {
            std::cerr << "neuron add <file>\n";
            return 1;
        }

        Repository repo(".neuron");
        Index index(".neuron");

        std::string file = argv[2];
        std::ifstream in(file, std::ios::binary);
        if (!in) {
            std::cerr << "cannot open file\n";
            return 1;
        }

        std::vector<uint8_t> data(
            (std::istreambuf_iterator<char>(in)),
            std::istreambuf_iterator<char>()
        );

        Object blob{ObjectType::Blob, data};
        std::string hash = repo.write_object(blob);

        index.add(file, hash);
        index.write();
    }

    else if (cmd == "commit") {
        if (argc < 3) {
            std::cerr << "neuron commit \"message\"\n";
            return 1;
        }

        Repository repo(".neuron");
        Index index(".neuron");

        std::string msg = argv[2];
        auto tree = build_tree(index.entries(), repo);
        std::string parent = read_ref(".neuron", "HEAD");

        std::string commit =
            create_commit(repo, tree, parent, msg);

        update_ref(".neuron", "HEAD", commit);
        std::cout << commit << "\n";
    }

    else if (cmd == "push") {
        if (argc < 3) {
            std::cerr << "neuron push ssh://user@host/repo.git\n";
            return 1;
        }
        return neuron_push(argv[2]);
    }
    else if (cmd == "pull") {
    if (argc < 3) {
        std::cerr << "neuron pull ssh://user@host/repo.git\n";
        return 1;
    }
    return neuron_pull(argv[2]);
}


    else {
        std::cerr << "unknown command\n";
        return 1;
    }

    return 0;
}

