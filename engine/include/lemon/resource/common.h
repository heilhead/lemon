#pragma once

#include <cstdint>
#include <folly/Hash.h>
#include <optional>
#include <string>

namespace lemon::res {
    enum class ResourceType {
        Unknown,
        Level,
        Bundle,
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
        InitializationError,
        MetadataMissing,
        DataMissing,
        DependencyError,
        DependencyMissing,
    };

    struct ResourceObjectHandle {
    public:
        static constexpr uint64_t InvalidHandle = 0;

        ResourceObjectHandle() : inner{InvalidHandle} {}

        ResourceObjectHandle(uint64_t hash) : inner{hash} {}

        ResourceObjectHandle(const std::string& file) {
            inner = folly::hash::fnva64(file);
        }

    private:
        uint64_t inner;

    public:
        inline uint64_t
        get() const {
            return inner;
        }

        inline operator uint64_t() const {
            return inner;
        }

        inline bool
        isValid() const {
            return inner != InvalidHandle;
        }
    };

    struct ResourceHandle : ResourceObjectHandle {
        ResourceHandle() : ResourceObjectHandle(InvalidHandle) {}
        ResourceHandle(uint64_t hash) : ResourceObjectHandle(hash) {}
        ResourceHandle(const std::string& file) : ResourceObjectHandle(file) {}
        ResourceState
        getState(ResourceObjectHandle object = ResourceObjectHandle());
    };
} // namespace lemon::res
