#pragma once
#include <string>
#include <stdexcept>

struct SshUrl {
    std::string user_host; // user@host
    std::string repo;      // repo.git
};

inline SshUrl parse_ssh_url(const std::string& url) {
    if (!url.starts_with("ssh://")) {
        throw std::runtime_error("invalid ssh url");
    }
    std::string rest = url.substr(6);
    auto slash = rest.find('/');
    if (slash == std::string::npos) {
        throw std::runtime_error("missing repo in ssh url");
    }
    return { rest.substr(0, slash), rest.substr(slash + 1) };
}

