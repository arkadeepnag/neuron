#pragma once
#include <string>
#include <unordered_set>

class Repository;

class ObjectGraph {
public:
    explicit ObjectGraph(Repository& repo);

    void mark_reachable(const std::string& start);
    const std::unordered_set<std::string>& reachable() const;

    // NEW
    bool is_ancestor(
        const std::string& ancestor,
        const std::string& descendant
    );

private:
    Repository& repo;
    std::unordered_set<std::string> seen;

    void walk(const std::string& hash);
};

