#pragma once
#include <vector>
#include <string>
#include <cstdint>

class Repository;

std::vector<uint8_t> create_pack(
    Repository& repo,
    const std::vector<std::string>& objects
);

void apply_pack(
    Repository& repo,
    const std::vector<uint8_t>& pack
);
