#include <iostream>
#include <filesystem>
#include <cstdio>
#include <vector>
#include <stdexcept>
#include <map>

#include "ssh_url.hpp"
#include "refs_remote.hpp"
#include "repository.hpp"
#include "refs.hpp"
#include "packfile.hpp"

namespace fs = std::filesystem;

static std::vector<uint8_t> read_all(FILE* f) {
    std::vector<uint8_t> data;
    uint8_t buf[4096];
    while (true) {
        size_t n = fread(buf, 1, sizeof(buf), f);
        if (n == 0) break;
        data.insert(data.end(), buf, buf + n);
    }
    return data;
}

static void init_local_repo(const fs::path& root) {
    fs::create_directories(root / ".neuron");
    fs::create_directories(root / ".neuron" / "objects");
    fs::create_directories(root / ".neuron" / "refs");
}

int neuron_clone(const std::string& url, const std::string& dir) {
    auto parsed = parse_ssh_url(url);

    // 1) Fetch remote refs
    auto refs = fetch_remote_refs(url);
    if (!refs.count("HEAD")) {
        throw std::runtime_error("remote has no HEAD (empty repo)");
    }
    std::string remote_head = refs["HEAD"];

    // 2) Determine target directory
    fs::path target = dir.empty()
        ? fs::path(parsed.repo).stem()
        : fs::path(dir);

    if (fs::exists(target)) {
        throw std::runtime_error("target directory already exists");
    }

    fs::create_directories(target);

    // 3) Initialize local repo layout
    init_local_repo(target);

    // 4) Create repository object
    Repository repo((target / ".neuron").string());

    // 5) Request full pack (no haves)
    std::string ssh_cmd =
        "ssh " + parsed.user_host +
        " neuron-upload-pack " + parsed.repo;

    FILE* in = popen(ssh_cmd.c_str(), "r");
    if (!in) {
        throw std::runtime_error("failed to start ssh upload-pack");
    }

    std::vector<uint8_t> pack = read_all(in);
    pclose(in);

    if (!pack.empty()) {
        apply_pack(repo, pack);
        update_ref((target / ".neuron").string(), "HEAD", remote_head);
    }

    std::cout << "Cloned into '" << target.string() << "'\n";
    return 0;
}

