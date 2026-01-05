#include "remote.h"
#include "utils.hpp"
#include <string>
#include <iostream>
#include <cstdlib>
#include <vector>
#include <sstream>
#include <filesystem>
#include <unistd.h>

static bool has_rsync()
{
    return system("which rsync > /dev/null 2>&1") == 0;
}

static bool has_ssh()
{
    return system("which ssh > /dev/null 2>&1") == 0;
}

// parse SSH-style remote into user@host and path parts when ':' present
static void split_ssh_remote(const std::string &remote, std::string &hostpart, std::string &remotepath)
{
    auto pos = remote.find(":");
    if (pos == std::string::npos)
    {
        hostpart.clear();
        remotepath = remote;
    }
    else
    {
        hostpart = remote.substr(0, pos);
        remotepath = remote.substr(pos + 1);
    }
}

int push_remote(const std::string &remote)
{
    if (!has_rsync())
    {
        std::cerr << "rsync is required for push/pull.\n";
        return 1;
    }

    std::string src = ".neuron/";

    // If remote contains ':' treat as remote SSH target, else local path
    std::string cmd_objects;
    std::string cmd_meta;
    if (remote.find(":") != std::string::npos)
    {
        // remote SSH target: copy objects ignoring existing, then copy refs/HEAD/index (overwrite)
        cmd_objects = "rsync -a --ignore-existing " + src + " '" + remote + "/.neuron/objects/'";
        cmd_meta = "rsync -a .neuron/refs '" + remote + ":/.neuron/' && rsync -a .neuron/HEAD '" + remote + ":/.neuron/' && rsync -a .neuron/index '" + remote + ":/.neuron/'";
    }
    else
    {
        // local path
        std::string mk = "mkdir -p '" + remote + "/.neuron/objects'";
        system(mk.c_str());
        cmd_objects = "rsync -a --ignore-existing " + src + " '" + remote + "/.neuron/objects/'";
        cmd_meta = "rsync -a .neuron/refs '" + remote + "/.neuron/' && rsync -a .neuron/HEAD '" + remote + "/.neuron/' && rsync -a .neuron/index '" + remote + "/.neuron/'";
    }

    std::cerr << "Running: " << cmd_objects << "\n";
    int rc = system(cmd_objects.c_str());
    if (rc != 0)
    {
        std::cerr << "push (objects) failed with code " << rc << "\n";
        return 1;
    }

    std::cerr << "Running: " << cmd_meta << "\n";
    rc = system(cmd_meta.c_str());
    if (rc != 0)
    {
        std::cerr << "push (meta) failed with code " << rc << "\n";
        return 1;
    }

    std::cout << "Push complete.\n";
    return 0;
}

int pull_remote(const std::string &remote)
{
    if (!has_rsync())
    {
        std::cerr << "rsync is required for push/pull.\n";
        return 1;
    }

    std::string dst = ".neuron/";
    std::string cmd_objects;
    std::string cmd_meta;
    if (remote.find(":") != std::string::npos)
    {
        // remote SSH target: pull objects ignoring existing, then pull refs/HEAD/index (overwrite)
        cmd_objects = "rsync -a --ignore-existing '" + remote + "/.neuron/objects/' " + dst + "objects/";
        cmd_meta = "rsync -a '" + remote + "/.neuron/refs' .neuron/ && rsync -a '" + remote + "/.neuron/HEAD' .neuron/ && rsync -a '" + remote + "/.neuron/index' .neuron/";
    }
    else
    {
        // local path
        cmd_objects = "rsync -a --ignore-existing '" + remote + "/.neuron/objects/' " + dst + "objects/";
        cmd_meta = "rsync -a '" + remote + "/.neuron/refs' .neuron/ && rsync -a '" + remote + "/.neuron/HEAD' .neuron/ && rsync -a '" + remote + "/.neuron/index' .neuron/";
    }

    std::cerr << "Running: " << cmd_objects << "\n";
    int rc = system(cmd_objects.c_str());
    if (rc != 0)
    {
        std::cerr << "pull (objects) failed with code " << rc << "\n";
        return 1;
    }

    std::cerr << "Running: " << cmd_meta << "\n";
    rc = system(cmd_meta.c_str());
    if (rc != 0)
    {
        std::cerr << "pull (meta) failed with code " << rc << "\n";
        return 1;
    }

    std::cout << "Pull complete.\n";
    return 0;
}

int test_remote(const std::string &remote)
{
    // local path
    if (remote.find(":") == std::string::npos)
    {
        if (!file_exists(remote))
        {
            std::cerr << "Local path not found: " << remote << "\n";
            return 1;
        }
        // check write perms
        if (access(remote.c_str(), W_OK) != 0)
        {
            std::cerr << "No write permission for: " << remote << "\n";
            return 1;
        }
        std::cout << "Local path reachable: " << remote << "\n";
        return 0;
    }

    if (!has_ssh())
    {
        std::cerr << "ssh is required to test SSH remotes.\n";
        return 1;
    }

    std::string hostpart, remotepath;
    split_ssh_remote(remote, hostpart, remotepath);
    // test SSH connectivity
    std::string cmd = "ssh -o BatchMode=yes -o ConnectTimeout=5 " + hostpart + " 'echo OK' 2>/dev/null";
    int rc = system(cmd.c_str());
    if (rc != 0)
    {
        std::cerr << "SSH remote not reachable: " << hostpart << "\n";
        return 1;
    }
    std::cout << "SSH remote reachable: " << hostpart << "\n";
    return 0;
}

int list_remote(const std::string &remote)
{
    if (remote.find(":") == std::string::npos)
    {
        std::string dir = remote + "/.neuron/refs/heads";
        if (!file_exists(dir))
        {
            std::cerr << "No refs/heads on remote path: " << dir << "\n";
            return 1;
        }
        // list files
        std::vector<std::string> files;
        for (auto &f : std::filesystem::directory_iterator(dir))
        {
            files.push_back(f.path().filename());
        }
        for (auto &n : files)
            std::cout << n << "\n";
        return 0;
    }

    if (!has_ssh())
    {
        std::cerr << "ssh required to list remote heads.\n";
        return 1;
    }

    std::string hostpart, remotepath;
    split_ssh_remote(remote, hostpart, remotepath);
    std::string cmd = "ssh " + hostpart + " 'ls -1 " + remotepath + "/.neuron/refs/heads 2>/dev/null || true'";
    int rc = system(cmd.c_str());
    return (rc == 0) ? 0 : 1;
}

int clone_remote(const std::string &remote, const std::string &target_dir)
{
    // create target
    std::string mk = "mkdir -p '" + target_dir + "'";
    system(mk.c_str());
    // pull into target_dir
    if (!has_rsync())
    {
        std::cerr << "rsync required to clone.\n";
        return 1;
    }
    std::string cmd;
    if (remote.find(":") != std::string::npos)
    {
        cmd = "rsync -a '" + remote + ":/.neuron/' '" + target_dir + "/.neuron/'";
    }
    else
    {
        cmd = "rsync -a '" + remote + "/.neuron/' '" + target_dir + "/.neuron/'";
    }
    std::cerr << "Running: " << cmd << "\n";
    int rc = system(cmd.c_str());
    if (rc != 0)
    {
        std::cerr << "clone failed: " << rc << "\n";
        return 1;
    }
    std::cout << "Cloned into " << target_dir << "\n";
    return 0;
}
