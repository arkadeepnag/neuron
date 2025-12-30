#pragma once

#include <string>
#include "object.hpp"

/*
 Repository
 ----------
 Responsible ONLY for:
 - writing immutable objects
 - reading loose objects
 - resolving repository root

 NO transport logic
 NO DB logic
 NO SSH logic
*/

class Repository {
public:
    explicit Repository(const std::string& repo_path);

    // Write a loose object (blob / tree / commit)
    // Returns SHA1 hash
    std::string write_object(const Object& obj);

    // Read a loose object by hash
    Object read_object(const std::string& hash);

    // Repo root path (.neuron)
    const std::string& root() const;

private:
    std::string repo_root;
};

