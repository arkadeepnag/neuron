#pragma once
#include <vector>
#include <string>

class Repository;

std::vector<std::string> walk_objects(
    Repository& repo,
    const std::vector<std::string>& roots
);
