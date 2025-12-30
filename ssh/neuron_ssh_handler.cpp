#include <iostream>
#include <cstdlib>
#include <string>
#include "pack_handlers.hpp"
#include "repository.hpp"
#include "database.hpp"
#include "pack_handlers.hpp"

static std::string env(const char* k) {
    const char* v = getenv(k);
    return v ? v : "";
}

int main(int /*argc*/, char** /*argv*/) {
    std::string cmd = env("SSH_ORIGINAL_COMMAND");
    if (cmd.empty()) {
        std::cerr << "No SSH command\n";
        return 1;
    }

    auto pos = cmd.find(' ');
    std::string op = cmd.substr(0, pos);
    std::string repo_name = cmd.substr(pos + 1);

    std::string fp = env("SSH_KEY_FINGERPRINT");
    if (fp.empty()) {
        std::cerr << "Missing SSH fingerprint\n";
        return 1;
    }

    Database db("neuron.db");

    auto user = db.user_from_ssh_key(fp);
    if (!user.valid) {
        std::cerr << "Unauthorized\n";
        return 1;
    }

    auto repo_meta = db.repo_by_name(repo_name);
    if (!repo_meta.valid) {
        std::cerr << "Repo not found\n";
        return 1;
    }

    bool can_write = db.has_access(user.id, repo_meta.id, "write");
    bool can_read  = db.has_access(user.id, repo_meta.id, "read");

    Repository repo(repo_meta.path);

    if (op == "neuron-upload-pack") {
        if (!can_read) {
            std::cerr << "Read denied\n";
            return 1;
        }
        handle_upload_pack(repo);
    }
    else if (op == "neuron-receive-pack") {
        if (!can_write) {
            std::cerr << "Write denied\n";
            return 1;
        }
        handle_receive_pack(repo);
    }
else if (op == "neuron-ls-refs") {
    if (!can_read) {
        std::cerr << "Read denied\n";
        return 1;
    }
    handle_ls_refs(repo);
}

    else {
        std::cerr << "Unknown command\n";
        return 1;
    }

    return 0;
}

