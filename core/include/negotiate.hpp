#pragma once
#include <string>
#include <vector>

class Repository;

struct NegotiationResult {
    std::vector<std::string> send_objects;
};

NegotiationResult negotiate(
    Repository& repo,
    const std::vector<std::string>& wants,
    const std::vector<std::string>& haves
);
