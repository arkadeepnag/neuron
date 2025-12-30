#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include <cstdio>
#include <memory>
#include <array>
#include <algorithm>
#include "repository.hpp"
#include "refs.hpp"
#include "graph.hpp"
#include "walker.hpp"
#include "packfile.hpp"


static void exec_with_input(
    const std::string& cmd,
    const std::vector<uint8_t>& input
) {
    FILE* pipe = popen(cmd.c_str(), "w");
    if (!pipe) {
        throw std::runtime_error("failed to open pipe");
    }

    fwrite(input.data(), 1, input.size(), pipe);
    pclose(pipe);
}

/*
  neuron push ssh://user@host/repo.git
*/
int neuron_push(const std::string& url) {
    // Parse ssh://user@host/repo.git
    if (!url.starts_with("ssh://")) {
        throw std::runtime_error("only ssh:// URLs supported");
    }

    std::string rest = url.substr(6);
    auto at = rest.find('@');
    auto slash = rest.find('/', at + 1);

    std::string user = rest.substr(0, at);
    std::string host = rest.substr(at + 1, slash - at - 1);
    std::string repo = rest.substr(slash + 1);

    Repository local(".neuron");

    // Local HEAD
    std::string head = read_ref(".neuron", "HEAD");
    if (head.empty()) {
        throw std::runtime_error("no commits to push");
    }

    // Collect all reachable objects
    ObjectGraph graph(local);
    graph.mark_reachable(head);

    std::vector<std::string> objects(
        graph.reachable().begin(),
        graph.reachable().end()
    );

    // Deterministic order
    std::sort(objects.begin(), objects.end());

    // Create pack
    auto pack = create_pack(local, objects);

    // SSH command
    std::string ssh_cmd =
        "ssh " + user + "@" + host +
        " neuron-receive-pack " + repo;

    // Send pack
    exec_with_input(ssh_cmd, pack);

    std::cout << "Push complete: " << repo << "\n";
    return 0;
}
