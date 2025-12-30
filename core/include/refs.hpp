#pragma once
#include <string>
#include <vector>
#include <utility>

// Read a ref (HEAD or refs/*)
std::string read_ref(
    const std::string& repo,
    const std::string& name
);

// Update a ref atomically
void update_ref(
    const std::string& repo,
    const std::string& name,
    const std::string& value
);

// List all refs (for advertisement)
std::vector<std::pair<std::string, std::string>>
list_refs(const std::string& repo);

