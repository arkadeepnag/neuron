#include <iostream>
#include <fstream>
#include <vector>
#include <filesystem>
#include "repository.hpp"
#include "index.hpp"
#include "tree.hpp"
#include "commit.hpp"
#include "refs.hpp"

int neuron_push(const std::string& url);
int neuron_pull(const std::string& url);
int neuron_clone(const std::string& url, const std::string& dir);

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
else if (cmd == "init") {
    bool bare = false;
    if (argc >= 3 && std::string(argv[2]) == "--bare") {
        bare = true;
    }

    std::filesystem::path root = bare ? "." : ".";
    std::filesystem::path neuron = root / ".neuron";

    if (std::filesystem::exists(neuron)) {
        std::cerr << "repository already initialized\n";
        return 1;
    }

    std::filesystem::create_directories(neuron / "objects");
    std::filesystem::create_directories(neuron / "refs");

    if (!bare) {
        std::ofstream(neuron / "refs" / "HEAD");
    }

    std::cout << "Initialized "
              << (bare ? "bare " : "")
              << "neuron repository\n";
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
  else if (cmd == "clone") {
            if (argc < 3) {
                std::cerr << "neuron clone ssh://user@host/repo.git [dir]\n";
                return 1;
            }
            std::string dir = (argc >= 4) ? argv[3] : "";
            return neuron_clone(argv[2], dir);
        }

    else {
        std::cerr << "unknown command\n";
        return 1;
    }

    return 0;
}

