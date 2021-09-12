#pragma once

#include <string>
#include <cstdint>
#include <optional>
#include <folly/Hash.h>

namespace lemon::res {
    enum class ResourceType {
        Unknown,
        Level,
        Model,
        Material,
        Texture,
        Mesh,
    };

    enum class ResourceState {
        NotLoaded,
        Loading,
        LoadingError,
        ObjectNotFound,
        Ready,
    };

    enum class ResourceLoadingError {
        Unknown,
    };

    using ResourceID = uint64_t;

    ResourceID getResourceID(std::string& file);
}
