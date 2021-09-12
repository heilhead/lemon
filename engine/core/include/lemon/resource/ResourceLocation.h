#pragma once

#include <filesystem>
#include <folly/hash/Hash.h>
#include <lemon/resource/common.h>

namespace lemon::res {
    static const char* kLocationObjectDelimiter = ":";

    struct ResourceLocation {
        ResourceID id;
        std::filesystem::path file;
        std::optional<std::string> object;

        explicit ResourceLocation(std::string& inLocation);
        ResourceLocation(std::string& inLocation, std::string&& inObject);

        // @TODO `ResourceState getState();`
    };
}
