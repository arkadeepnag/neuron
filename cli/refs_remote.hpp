#pragma once
#include <map>
#include <string>

std::map<std::string, std::string>
fetch_remote_refs(const std::string& url);

