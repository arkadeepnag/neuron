#include "refs.hpp"
#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

std::string read_ref(
    const std::string& repo,
    const std::string& name
) {
    fs::path p = fs::path(repo) / "refs" / name;
    if (!fs::exists(p)) return "";

    std::ifstream in(p);
    std::string hash;
    std::getline(in, hash);
    return hash;
}

void update_ref(
    const std::string& repo,
    const std::string& name,
    const std::string& value
) {
    fs::path p = fs::path(repo) / "refs" / name;
    fs::create_directories(p.parent_path());

    fs::path tmp = p;
    tmp += ".tmp";

    {
        std::ofstream out(tmp);
        out << value << "\n";
        out.flush();
    }

    fs::rename(tmp, p);
}

std::vector<std::pair<std::string, std::string>>
list_refs(const std::string& repo) {
    std::vector<std::pair<std::string, std::string>> refs;

    fs::path refs_dir = fs::path(repo) / "refs";
    if (!fs::exists(refs_dir)) return refs;

    for (auto& e : fs::recursive_directory_iterator(refs_dir)) {
        if (!e.is_regular_file()) continue;

        std::ifstream in(e.path());
        std::string hash;
        std::getline(in, hash);

        std::string name =
            fs::relative(e.path(), refs_dir).string();

        refs.emplace_back("refs/" + name, hash);
    }

    std::string head = read_ref(repo, "HEAD");
    if (!head.empty()) {
        refs.insert(refs.begin(), {"HEAD", head});
    }

    return refs;
}

