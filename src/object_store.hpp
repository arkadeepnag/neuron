#pragma once
#include <string>
#include "blockstore.h"

// Store an object (type + content) into the blockstore and return its hash
std::string store_object(
    const std::string &type,
    const std::string &content,
    BlockStore *bs = nullptr);
