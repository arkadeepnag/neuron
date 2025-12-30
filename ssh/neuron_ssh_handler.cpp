#include <iostream>
#include <sstream>
#include <cstdlib>

#include "database.hpp"
#include "repository.hpp"
#include "pack_handlers.hpp"

static std::string must_env(const char* name) {
    const char* v = std::getenv(name);
    if (!v) throw std::runtime_error("Missing SSH_ORIGINAL_COMMAND");
    return v;
}

int main() {
    try {
        std::string original = must_env("SSH_ORIGINAL_COMMAND");

        std::istringstream iss(original);
        std::string op, repo_name;
        iss >> op >> repo_name;

        if (repo_name.empty()) {
            throw std::runtime_error("Missing repo name");
        }

        Database db("/var/lib/neuron/neuron.db");
        Repository repo("/var/lib/neuron/repos/" + repo_name);

        // Authorization already enforced at auth time
        if (op == "neuron-ls-refs") {
            handle_ls_refs(repo);
        }
        else if (op == "neuron-upload-pack") {
            handle_upload_pack(repo);
        }
        else if (op == "neuron-receive-pack") {
            handle_receive_pack(repo);
        }
        else {
            throw std::runtime_error("Unknown command");
        }

        return 0;
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << "\n";
        return 1;
    }
}

