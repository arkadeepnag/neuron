#pragma once
#include <string>

// Push local .neuron state to remote (remote may be a local path or user@host:/path)
int push_remote(const std::string &remote);

// Pull remote .neuron state into local (remote may be a local path or user@host:/path)
int pull_remote(const std::string &remote);

// Test connectivity to a remote. Returns 0 if reachable, non-zero otherwise.
int test_remote(const std::string &remote);

// List branches available on remote. Returns 0 on success.
int list_remote(const std::string &remote);

// Clone remote into target directory (creates target_dir/.neuron)
int clone_remote(const std::string &remote, const std::string &target_dir);
