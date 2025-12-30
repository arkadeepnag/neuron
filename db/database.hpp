#pragma once
#include <string>
#include <sqlite3.h>

struct User {
    int id;
    bool valid;
};

struct Repo {
    int id;
    std::string path;
    bool valid;
};

class Database {
public:
    explicit Database(const std::string& file);
    ~Database();

    User user_from_ssh_key(const std::string& fingerprint);
    Repo repo_by_name(const std::string& name);
    bool has_access(int user_id, int repo_id, const std::string& role);

private:
    sqlite3* db;  
};

