#pragma once

#include <filesystem>
#include <folly/hash/Hash.h>
#include <lemon/resource/ResourceContract.h>
#include <lemon/resource/ResourceLocation.h>
#include <lemon/resource/ResourceMetadata.h>
#include <lemon/resource/ResourceStore.h>
#include <lemon/shared/filesystem.h>

namespace lemon::res {
    struct ResourceContract;

    struct ResourceRequestDescriptor {
        ResourceLocation location;
        ResourceLifetime lifetime;
    };

    class ResourceManager {
    public:
        explicit ResourceManager(std::filesystem::path&& rootPath);
        ~ResourceManager();

        static ResourceManager*
        get();

    private:
        ResourceStore store;
        std::filesystem::path root;

    public:
        std::filesystem::path
        resolvePath(const struct ResourceLocation& location);

        ResourceStore&
        getStore() {
            return store;
        }

        template<typename TResource>
        tl::expected<ResourceMetadata, ResourceLoadingError>
        loadMetadata(const ResourceLocation& location);

        ResourceContract*
        getContract(ResourceHandle handle);

        template<typename TResource>
        TResource*
        getResource(ResourceHandle handle);

        ResourceState
        getResourceState(ResourceHandle handle, ResourceObjectHandle object = ResourceHandle::InvalidHandle);

        template<typename T>
        ResourceContract::FutureType<T>
        loadResource(const ResourceLocation& location, ResourceLifetime lifetime = ResourceLifetime::Short);
    };
} // namespace lemon::res

#include <lemon/resource/ResourceManager.inl>
