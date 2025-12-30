#pragma once
#include <string>
#include <stdexcept>
struct SshUrl {
    std::string user_host;  // user@host
    std::string repo;       // repo.git
};

inline SshUrl parse_ssh_url(const std::string& url) {
    // ssh://user@host/repo.git
    if (!url.starts_with("ssh://")) {
        throw std::runtime_error("invalid ssh url");
    }

    std::string rest = url.substr(6);
    auto slash = rest.find('/');
    if (slash == std::string::npos) {
        throw std::runtime_error("missing repo in ssh url");
    }

    return {
        rest.substr(0, slash),   // user@host
        rest.substr(slash + 1)   // repo.git
    };
}
