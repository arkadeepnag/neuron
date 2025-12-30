#pragma once
#include <string>
#include <map>

class Repository;

std::string build_tree(
    const std::map<std::string, std::string>& index,
    Repository& repo
);

