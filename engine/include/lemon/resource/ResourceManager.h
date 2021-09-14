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

    using FactoryResultType = folly::coro::Task<ResourceContract::ResolutionType<ResourceInstance>>;
    using ResourceFactoryFn = FactoryResultType (*)(const std::string& ref, ResourceLifetime lifetime);

    class ResourceManager {
    public:
        explicit ResourceManager(std::filesystem::path&& rootPath);
        ~ResourceManager();

        static ResourceManager*
        get();

    private:
        ResourceStore store;
        std::filesystem::path root;
        std::unordered_map<ResourceClassID, ResourceFactoryFn> factories;

    public:
        std::filesystem::path
        resolvePath(const struct ResourceLocation& location);

        ResourceStore&
        getStore() {
            return store;
        }

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

        template<typename TResource>
        tl::expected<ResourceMetadata, ResourceLoadingError>
        loadMetadata(const ResourceLocation& location);

        template<typename TResource>
        static ResourceClassID
        getClassID() {
            static std::string_view strName{typeid(TResource).name()};
            static auto hash = folly::hash::fnv64_buf(strName.data(), strName.size());
            return hash;
        }

        template<typename TResource>
        void
        registerClass();

        std::optional<ResourceFactoryFn>
        getFactoryFn(ResourceClassID id);
    };
} // namespace lemon::res

#include <lemon/resource/ResourceManager.inl>
