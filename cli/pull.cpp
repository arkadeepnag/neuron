#include <iostream>
#include <vector>
#include <string>
#include <cstdio>
#include <cstdlib>
#include <algorithm>
#include <map>

#include "repository.hpp"
#include "refs.hpp"
#include "graph.hpp"
#include "packfile.hpp"
#include "refs_remote.hpp"
#include "ssh_url.hpp"

static std::vector<uint8_t>
read_all(FILE* f) {
    std::vector<uint8_t> data;
    uint8_t buf[4096];
    while (true) {
        size_t n = fread(buf, 1, sizeof(buf), f);
        if (n == 0) break;
        data.insert(data.end(), buf, buf + n);
    }
    return data;
}

int neuron_pull(const std::string& url) {
    auto parsed = parse_ssh_url(url);

    // 1. Fetch remote refs
    auto remote_refs = fetch_remote_refs(url);

    if (!remote_refs.count("HEAD")) {
        throw std::runtime_error("remote has no HEAD");
    }

    std::string remote_head = remote_refs["HEAD"];
    std::string local_head  = read_ref(".neuron", "HEAD");

    Repository repo(".neuron");
    ObjectGraph graph(repo);

    // 2. Already up to date
    if (local_head == remote_head) {
        std::cout << "Already up to date\n";
        return 0;
    }

    // 3. Fast-forward safety check
    if (!local_head.empty() &&
        !graph.is_ancestor(local_head, remote_head)) {
        std::cerr << "Diverged history. Fast-forward refused.\n";
        return 1;
    }

    // 4. Build have list
    std::string proto;
    if (!local_head.empty()) {
        graph.mark_reachable(local_head);
        for (auto& h : graph.reachable()) {
            proto += "have " + h + "\n";
        }
    }
    proto += "done\n";

    // 5. Request pack
    std::string ssh_cmd =
        "ssh " + parsed.user_host +
        " neuron-upload-pack " + parsed.repo;

    FILE* out = popen(ssh_cmd.c_str(), "w");
    if (!out) throw std::runtime_error("ssh failed");

    fwrite(proto.data(), 1, proto.size(), out);
    pclose(out);

    FILE* in = popen(ssh_cmd.c_str(), "r");
    if (!in) throw std::runtime_error("ssh failed");

    auto pack = read_all(in);
    pclose(in);

    if (pack.empty()) {
        std::cout << "Already up to date\n";
        return 0;
    }

    // 6. Apply pack
    apply_pack(repo, pack);

    // 7. SAFE fast-forward
    update_ref(".neuron", "HEAD", remote_head);

    std::cout << "Fast-forwarded to " << remote_head << "\n";
    return 0;
}

