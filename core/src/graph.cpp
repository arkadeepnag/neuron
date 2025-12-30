#include "graph.hpp"
#include "repository.hpp"
#include "object.hpp"

#include <sstream>

ObjectGraph::ObjectGraph(Repository& r)
    : repo(r) {}

void ObjectGraph::mark_reachable(const std::string& start) {
    walk(start);
}

const std::unordered_set<std::string>&
ObjectGraph::reachable() const {
    return seen;
}

void ObjectGraph::walk(const std::string& hash) {
    if (hash.empty() || seen.count(hash)) return;
    seen.insert(hash);

    Object obj = repo.read_object(hash);
    if (obj.type != ObjectType::Commit) return;

    std::string body(obj.data.begin(), obj.data.end());
    std::istringstream ss(body);

    std::string line;
    while (std::getline(ss, line)) {
        if (line.starts_with("parent ")) {
            walk(line.substr(7));
        }
    }
}

bool ObjectGraph::is_ancestor(
    const std::string& ancestor,
    const std::string& descendant
) {
    seen.clear();
    walk(descendant);
    return seen.count(ancestor) > 0;
}

