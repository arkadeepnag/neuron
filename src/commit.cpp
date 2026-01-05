#include "commit.h"
#include "tree.h"
#include "utils.hpp"
#include <ctime>
#include <sstream>
#include "object_store.hpp"
#include <cstdio>
#include <vector>
#include <string>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <cstdlib>

void create_commit(const std::string &message)
{
    std::string tree_hash = write_tree();

    std::string parent = read_file(".neuron/refs/heads/main");
    parent.erase(parent.find_last_not_of("\n") + 1);
    // Build commit headers (without signature)
    std::ostringstream headers;
    headers << "tree " << tree_hash << "\n";
    if (!parent.empty())
        headers << "parent " << parent << "\n";

    std::time_t t = std::time(nullptr);
    headers << "author Arkadeep <user@neuron> " << t << "\n";

    std::string commit_no_sig = headers.str() + "\n" + message + "\n";

    // Attempt to sign if a key is configured
    std::string gpg_key;
    if (file_exists(".neuron/gpg_key"))
        gpg_key = read_file(".neuron/gpg_key");
    else if (const char *env = std::getenv("NEURON_GPG_KEY"))
        gpg_key = env;

    if (!gpg_key.empty())
    {
        // trim newline
        gpg_key.erase(gpg_key.find_last_not_of("\n") + 1);

        // write commit content to temp file
        char tmpname[] = "/tmp/neuron_commit_XXXXXX";
        int fd = mkstemp(tmpname);
        if (fd != -1)
        {
            write(fd, commit_no_sig.data(), commit_no_sig.size());
            close(fd);

            // Run gpg to create detached armored signature to stdout
            std::string cmd = "gpg --batch --yes --armor --detach-sign --local-user '" + gpg_key + "' --output - " + tmpname + " 2>/dev/null";
            FILE *p = popen(cmd.c_str(), "r");
            if (p)
            {
                std::vector<char> buf;
                char chunk[4096];
                while (true)
                {
                    size_t n = fread(chunk, 1, sizeof(chunk), p);
                    if (n == 0)
                        break;
                    buf.insert(buf.end(), chunk, chunk + n);
                }
                pclose(p);

                if (!buf.empty())
                {
                    std::string sig(buf.begin(), buf.end());
                    // normalize and trim
                    while (!sig.empty() && (sig.back() == '\n' || sig.back() == '\r'))
                        sig.pop_back();

                    // Insert gpgsig header with proper continued-line format
                    std::istringstream ss(sig);
                    std::string line;
                    std::ostringstream final;
                    final << headers.str();
                    bool first = true;
                    while (std::getline(ss, line))
                    {
                        if (first)
                        {
                            final << "gpgsig " << line << "\n";
                            first = false;
                        }
                        else
                        {
                            final << " " << line << "\n";
                        }
                    }

                    final << "\n"
                          << message << "\n";

                    std::string commit_with_sig = final.str();
                    std::string commit_hash = store_object("commit", commit_with_sig);
                    write_file(".neuron/refs/heads/main", commit_hash + "\n");
                    write_file(".neuron/index", ""); // clear staging
                    printf("Committed %s (signed)\n", commit_hash.c_str());

                    // cleanup temp
                    unlink(tmpname);
                    return;
                }

                // else fallthrough to unsigned commit
            }
            unlink(tmpname);
        }
    }

    // Fallback: unsigned commit
    std::string commit_hash = store_object("commit", commit_no_sig);
    write_file(".neuron/refs/heads/main", commit_hash + "\n");

    write_file(".neuron/index", ""); // clear staging
    printf("Committed %s\n", commit_hash.c_str());
}
