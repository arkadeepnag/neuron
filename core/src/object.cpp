#include "object.hpp"

std::string object_type_name(ObjectType t) {
    switch (t) {
        case ObjectType::Blob: return "blob";
        case ObjectType::Tree: return "tree";
        case ObjectType::Commit: return "commit";
    }
    return "";
}

std::string serialize_object(const Object& obj) {
    std::string header =
        object_type_name(obj.type) + " " +
        std::to_string(obj.data.size()) + '\0';

    return header + std::string(obj.data.begin(), obj.data.end());
}
