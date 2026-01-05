#pragma once
#include <string>

std::string get_head_ref();
std::string get_head_commit();
void update_head_commit(const std::string &hash);
