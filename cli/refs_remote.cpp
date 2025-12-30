#include "refs_remote.hpp"
#include "ssh_url.hpp"

#include <cstdio>
#include <sstream>
#include <stdexcept>

std::map<std::string, std::string>
fetch_remote_refs(const std::string& url) {
    std::map<std::string, std::string> refs;

    auto parsed = parse_ssh_url(url);

    std::string cmd =
        "ssh " + parsed.user_host +
        " neuron-ls-refs " + parsed.repo;

    FILE* pipe = popen(cmd.c_str(), "r");
    if (!pipe) {
        throw std::runtime_error("ssh failed");
    }

    char line[256];
    while (fgets(line, sizeof(line), pipe)) {
        std::istringstream iss(line);
        std::string name, hash;
        iss >> name >> hash;
        if (!name.empty() && !hash.empty()) {
            refs[name] = hash;
        }
    }

    pclose(pipe);
    return refs;
}

