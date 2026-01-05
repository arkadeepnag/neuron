#include <iostream>
#include <string>
#include "repo.h"
#include "remote.h"
#include "object_store.hpp"
#include "utils.hpp"
#include "index.hpp"
#include "commit.h"
#include "repo.h"
#include "checkout.h"
#include "refs.h"

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        std::cerr << "neuron <command>\n";
        return 1;
    }

    std::string cmd = argv[1];

    if (cmd == "init")
    {
        repo_init();
    }
    else if (cmd == "hash-object")
    {
        if (argc < 3)
        {
            std::cerr << "Usage: neuron hash-object <file>\n";
            return 1;
        }

        std::string content = read_file(argv[2]);
        std::string hash = store_object("blob", content);
        std::cout << hash << "\n";
    }
    else if (cmd == "add")
    {
        if (argc < 3)
        {
            std::cerr << "Usage: neuron add <file>\n";
            return 1;
        }

        index_add(argv[2]);
    }
    else if (cmd == "commit")
    {
        if (argc < 3)
        {
            std::cerr << "Usage: neuron commit <message>\n";
            return 1;
        }

        create_commit(argv[2]);
    }
    else if (cmd == "log")
    {
        repo_log();
    }
    else if (cmd == "checkout")
    {
        checkout_commit(argv[2]);
        update_head_commit(argv[2]);
    }
    else if (cmd == "branch")
    {
        create_branch(argv[2]);
    }
    else if (cmd == "remote-test")
    {
        if (argc < 3)
        {
            std::cerr << "Usage: neuron remote-test <remote>\n";
            return 1;
        }
        return test_remote(argv[2]);
    }
    else if (cmd == "remote-list")
    {
        if (argc < 3)
        {
            std::cerr << "Usage: neuron remote-list <remote>\n";
            return 1;
        }
        return list_remote(argv[2]);
    }
    else if (cmd == "clone")
    {
        if (argc < 4)
        {
            std::cerr << "Usage: neuron clone <remote> <target-dir>\n";
            return 1;
        }
        return clone_remote(argv[2], argv[3]);
    }
    else if (cmd == "push")
    {
        if (argc < 3)
        {
            std::cerr << "Usage: neuron push <remote>\n";
            return 1;
        }
        return push_remote(argv[2]);
    }
    else if (cmd == "pull")
    {
        if (argc < 3)
        {
            std::cerr << "Usage: neuron pull <remote>\n";
            return 1;
        }
        return pull_remote(argv[2]);
    }

    else
    {
        std::cerr << "Unknown command\n";
    }

    return 0;
}
