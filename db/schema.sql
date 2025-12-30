CREATE TABLE users (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    username TEXT UNIQUE NOT NULL,
    password_hash TEXT NOT NULL
);

CREATE TABLE ssh_keys (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    user_id INTEGER NOT NULL,
    public_key TEXT NOT NULL,
    fingerprint TEXT UNIQUE NOT NULL,
    FOREIGN KEY(user_id) REFERENCES users(id)
);

CREATE TABLE repositories (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    name TEXT NOT NULL,
    owner_id INTEGER NOT NULL,
    visibility TEXT CHECK(visibility IN ('public','private')),
    path_on_disk TEXT NOT NULL,
    FOREIGN KEY(owner_id) REFERENCES users(id)
);

CREATE TABLE repo_access (
    user_id INTEGER NOT NULL,
    repo_id INTEGER NOT NULL,
    role TEXT CHECK(role IN ('owner','write','read')),
    PRIMARY KEY(user_id, repo_id)
);
