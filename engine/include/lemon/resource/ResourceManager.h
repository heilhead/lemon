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

        /// <summary>
        /// Returns the singleton instance of `ResourceManager` if there's one, or `nullptr` otherwise.
        /// `ResourceManager` still needs to be manually instantiated somewhere for singleton to be used.
        /// </summary>
        /// <returns>Resource manager pointer, or `nullptr`</returns>
        static ResourceManager*
        get();

    private:
        ResourceStore store;
        std::filesystem::path root;
        std::unordered_map<ResourceClassID, ResourceFactoryFn> factories;

    public:
        /// <summary>
        /// Resolves `ResourceLocation` to the absolute file system path;
        /// </summary>
        /// <param name="location">Resource location</param>
        /// <returns>Absolute file system path</returns>
        std::filesystem::path
        resolvePath(const struct ResourceLocation& location);

        /// <summary>
        /// Returns the underlying store. Used internally.
        /// </summary>
        /// <returns>Resource store</returns>
        inline ResourceStore&
        getStore() {
            return store;
        }

        /// <summary>
        /// Returns contract for the specified resource, or `nullptr` if it doesn't exist.
        /// Note that the contract may not yet be fulfilled, i.e. if the resource is being loaded.
        /// </summary>
        /// <param name="handle">Resource handle</param>
        /// <returns>Resource contract pointer, or `nullptr` if not found</returns>
        ResourceContract*
        getContract(ResourceHandle handle);

        /// <summary>
        /// Returns the resource pointer, assuming it's loaded and ready.
        /// </summary>
        /// <typeparam name="TResource">`ResourceInstance` subclass</typeparam>
        /// <param name="handle">Resource handle</param>
        /// <returns>Resource instance pointer or `nullptr` if it's not available</returns>
        template<class TResource = ResourceInstance>
        TResource*
        getResource(ResourceHandle handle);

        /// <summary>
        /// Fetches resource state.
        /// </summary>
        /// <param name="handle">Resource handle</param>
        /// <param name="object">Optional resource object handle</param>
        /// <returns>The resource state</returns>
        ResourceState
        getResourceState(ResourceHandle handle, ResourceObjectHandle object = ResourceHandle::InvalidHandle);

        /// <summary>
        /// Attempts to load a resource and returns a future that resolves to either the resource, or
        /// an error. Returns existing resource if it's currently loading or already loaded.
        /// </summary>
        /// <typeparam name="TResource">`ResourceInstance` subclass</typeparam>
        /// <param name="location">Resource location</param>
        /// <param name="lifetime">Expected resource lifetime. The lifetime will only be used if the resource
        /// is not yet created</param>
        /// <returns>`folly::Future` which resolves to either the resource instance pointer, or an
        /// `ResourceLoadingError`</returns>
        template<class TResource>
        ResourceContract::FutureType<TResource>
        loadResource(const ResourceLocation& location, ResourceLifetime lifetime = ResourceLifetime::Short);

        /// <summary>
        /// Attempts to load a resource and returns a future that resolves to either the resource, or
        /// an error. Returns existing resource if it's currently loading or already loaded.
        /// </summary>
        /// <param name="id">Resource class ID</param>
        /// <param name="location">Resource location</param>
        /// <param name="lifetime">Expected resource lifetime. The lifetime will only be used if the resource
        /// is not yet created</param>
        /// <returns>`folly::Future` which resolves to either the resource instance pointer, or an
        /// `ResourceLoadingError`</returns>
        ResourceContract::FutureType<ResourceInstance>
        loadResource(ResourceClassID id, const ResourceLocation& location,
                     ResourceLifetime lifetime = ResourceLifetime::Short);

        /// <summary>
        /// Attempts to unload a resource identified by `ResourceLocation`.
        ///
        /// Can fail in two cases:
        /// - The resource is not fully loaded (i.e. its state is not `ResourceState::Ready`);
        /// - The resource is used as a dependency by another resource.
        /// </summary>
        /// <param name="location">Resource location</param>
        /// <returns>Whether the resource has been unloaded</returns>
        bool
        unloadResource(const ResourceLocation& location);

        /// <summary>
        /// Attempts to unload a resource identified by `ResourceHandle`.
        ///
        /// Can fail in two cases:
        /// - The resource is not fully loaded (i.e. its state is not `ResourceState::Ready`);
        /// - The resource is used as a dependency by another resource.
        /// </summary>
        /// <param name="handle">Resource handle</param>
        /// <returns>Whether the resource has been unloaded</returns>
        bool
        unloadResource(ResourceHandle handle);

        /// <summary>
        /// Loads metadata associated with the specified resource.
        /// </summary>
        /// <typeparam name="TResource">A `ResourceInstance` sublcass</typeparam>
        /// <param name="location">Resource location</param>
        /// <returns>Either `ResourceMetadata` or an error</returns>
        template<class TResource>
        tl::expected<ResourceMetadata, ResourceLoadingError>
        loadMetadata(const ResourceLocation& location);

        /// <summary>
        /// Creates a hash-based class ID. Note that the ID is based on the mangled C++ class name
        /// and will change if the class is renamed, moved to another namespace etc.
        /// </summary>
        /// <typeparam name="TResource">`ResourceInstance` subclass</typeparam>
        /// <returns>Hash-based class ID (`uint64_t`)</returns>
        template<class TResource>
        static ResourceClassID
        getClassID() {
            static std::string_view strName{typeid(TResource).name()};
            static auto hash = folly::hash::fnv64_buf(strName.data(), strName.size());
            return hash;
        }

        /// <summary>
        /// Registers a factory for a resource class. To qualify for the resource factory, the class
        /// must inherit from `ResourceInstance` and implement resource metadata traits. See `BundleResource`
        /// for an example of this implementation.
        /// </summary>
        /// <typeparam name="TResource">A `ResourceInstance` subclass</typeparam>
        template<class TResource>
        void
        registerClass();

        /// <summary>
        /// Finds a resource factory for the given class ID. Used internally.
        /// </summary>
        /// <param name="id">Class ID</param>
        /// <returns>A factory function if one is found</returns>
        std::optional<ResourceFactoryFn>
        getFactoryFn(ResourceClassID id);
    };
} // namespace lemon::res

#include <lemon/resource/ResourceManager.inl>
