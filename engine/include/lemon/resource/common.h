#pragma once

namespace lemon::res {
    using ResourceClassID = uint64_t;

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
        DataDecodingError,
        DependencyError,
        DependencyMissing,
        ObjectMissing,
        FactoryMissing,
    };

    struct ResourceObjectHandle {
    public:
        static constexpr uint64_t InvalidHandle = 0;

        ResourceObjectHandle() : inner{InvalidHandle} {}

        ResourceObjectHandle(uint64_t hash) : inner{hash} {}

        ResourceObjectHandle(const std::string& name)
        {
            inner = folly::hash::fnva64(name);
        }

    private:
        uint64_t inner;

    public:
        inline uint64_t
        get() const
        {
            return inner;
        }

        inline operator uint64_t() const
        {
            return inner;
        }

        inline bool
        isValid() const
        {
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

    /// <summary>
    /// Creates a hash-based class ID. Note that the ID can change if the class is renamed, moved to another
    /// namespace etc.
    /// </summary>
    /// <typeparam name="TResource">`ResourceInstance` subclass</typeparam>
    /// <returns>Hash-based class ID (`uint64_t`)</returns>
    template<class TResource>
    inline static ResourceClassID
    getClassID()
    {
        return utils::getTypeID<TResource>();
    }
} // namespace lemon::res

template<>
struct folly::hasher<lemon::res::ResourceObjectHandle> {
    using folly_is_avalanching = std::true_type;

    size_t
    operator()(const lemon::res::ResourceObjectHandle& data) const
    {
        return data.get();
    }
};

template<>
struct folly::hasher<lemon::res::ResourceHandle> {
    using folly_is_avalanching = std::true_type;

    size_t
    operator()(const lemon::res::ResourceHandle& data) const
    {
        return data.get();
    }
};
