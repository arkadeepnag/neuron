#include "negotiate.hpp"
#include "graph.hpp"
#include <unordered_set>

NegotiationResult negotiate(
    Repository& repo,
    const std::vector<std::string>& wants,
    const std::vector<std::string>& haves
) {
    ObjectGraph want_graph(repo);
    ObjectGraph have_graph(repo);

    for (auto& w : wants) want_graph.mark_reachable(w);
    for (auto& h : haves) have_graph.mark_reachable(h);

    std::unordered_set<std::string> have_set = have_graph.reachable();
    std::vector<std::string> needed;

    for (auto& obj : want_graph.reachable()) {
        if (!have_set.count(obj))
            needed.push_back(obj);
    }

    return {needed};
}
