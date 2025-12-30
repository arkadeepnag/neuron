#pragma once
#include <string>

// ✅ Forward declaration (REQUIRED)
class Repository;

std::string create_commit(
    Repository& repo,
    const std::string& tree_hash,
    const std::string& parent,
    const std::string& message
);

