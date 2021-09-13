#pragma once

#include <filesystem>
#include <folly/hash/Hash.h>
#include <lemon/resource/common.h>

namespace lemon::res {
    static const char* kLocationObjectDelimiter = ":";

    struct ResourceLocation {
        ResourceHandle handle;
        std::filesystem::path file;
        std::optional<std::string> object;

        explicit ResourceLocation(const std::string& inLocation);
        ResourceLocation(const std::string& inLocation, std::string&& inObject);
    };
} // namespace lemon::res
