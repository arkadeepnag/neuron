#include "commit.hpp"
#include "repository.hpp"   // ✅ FULL TYPE HERE
#include "object.hpp"

#include <ctime>
#include <sstream>

std::string create_commit(
    Repository& repo,
    const std::string& tree_hash,
    const std::string& parent,
    const std::string& message
) {
    std::stringstream ss;
    ss << "tree " << tree_hash << "\n";

    if (!parent.empty()) {
        ss << "parent " << parent << "\n";
    }

    ss << "author neuron <neuron@local> "
       << std::time(nullptr) << " +0000\n\n";

    ss << message << "\n";

    Object commit;
    commit.type = ObjectType::Commit;
    std::string body = ss.str();
    commit.data.assign(body.begin(), body.end());

    return repo.write_object(commit);
}

