#pragma once
#include <string>
#include <vector>
#include <cstdint>   // ✅ REQUIRED

enum class ObjectType { Blob, Tree, Commit };

struct Object {
    ObjectType type;
    std::vector<uint8_t> data;
};

std::string object_type_name(ObjectType t);
std::string serialize_object(const Object& obj);

