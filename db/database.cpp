#include "database.hpp"
#include <stdexcept>

Database::Database(const std::string& file) : db(nullptr) {
    if (sqlite3_open(file.c_str(), &db) != SQLITE_OK) {
        throw std::runtime_error("Failed to open database");
    }
}

Database::~Database() {
    if (db) sqlite3_close(db);
}

User Database::user_from_ssh_key(const std::string& fp) {
    sqlite3_stmt* stmt;
    User u{0, false};

    sqlite3_prepare_v2(
        db,
        "SELECT users.id FROM users "
        "JOIN ssh_keys ON users.id = ssh_keys.user_id "
        "WHERE ssh_keys.fingerprint = ?",
        -1, &stmt, nullptr
    );
    sqlite3_bind_text(stmt, 1, fp.c_str(), -1, SQLITE_TRANSIENT);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        u.id = sqlite3_column_int(stmt, 0);
        u.valid = true;
    }
    sqlite3_finalize(stmt);
    return u;
}

Repo Database::repo_by_name(const std::string& name) {
    sqlite3_stmt* stmt;
    Repo r{0, "", false};

    sqlite3_prepare_v2(
        db,
        "SELECT id, path_on_disk FROM repositories WHERE name = ?",
        -1, &stmt, nullptr
    );
    sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_TRANSIENT);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        r.id = sqlite3_column_int(stmt, 0);
        r.path = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        r.valid = true;
    }
    sqlite3_finalize(stmt);
    return r;
}

bool Database::has_access(int user_id, int repo_id, const std::string& role) {
    sqlite3_stmt* stmt;

    sqlite3_prepare_v2(
        db,
        "SELECT 1 FROM repo_access "
        "WHERE user_id=? AND repo_id=? "
        "AND (role=? OR role='owner')",
        -1, &stmt, nullptr
    );
    sqlite3_bind_int(stmt, 1, user_id);
    sqlite3_bind_int(stmt, 2, repo_id);
    sqlite3_bind_text(stmt, 3, role.c_str(), -1, SQLITE_TRANSIENT);

    bool ok = (sqlite3_step(stmt) == SQLITE_ROW);
    sqlite3_finalize(stmt);
    return ok;
}

