#include "walker.hpp"
#include "graph.hpp"
#include <algorithm>

std::vector<std::string> walk_objects(
    Repository& repo,
    const std::vector<std::string>& roots
) {
    ObjectGraph graph(repo);
    for (auto& r : roots)
        graph.mark_reachable(r);

    std::vector<std::string> objs(
        graph.reachable().begin(),
        graph.reachable().end()
    );

    std::sort(objs.begin(), objs.end()); // deterministic
    return objs;
}
