#pragma once

#include <filesystem>
#include <folly/hash/Hash.h>
#include <lemon/shared/filesystem.h>
#include <lemon/resource/ResourceStore.h>
#include <lemon/resource/ResourceLocation.h>
#include <lemon/resource/ResourceMetadata.h>

namespace lemon::res {
    class ResourceManager {
    public:
        explicit ResourceManager(std::filesystem::path&& rootPath);
        ~ResourceManager();

        static ResourceManager* get();

    private:
        ResourceStore store;
        std::filesystem::path root;

    public:
        std::filesystem::path resolvePath(const struct ResourceLocation& location);

        ResourceStore& getStore() {
            return store;
        }

        template<typename TResource>
        tl::expected<ResourceMetadata, ResourceLoadingError>
        loadMetadata(std::string& path);
    };

    template<typename TResource>
    tl::expected<ResourceMetadata, ResourceLoadingError>
    ResourceManager::loadMetadata(std::string& path) {
        auto location = ResourceLocation(path);
        auto fullPath = resolvePath(location);
        fullPath += ".meta";

        return lemon::io::readTextFile(fullPath)
            .map_error([](auto&& err) {
                return ResourceLoadingError::Unknown;
            })
            .map([](auto&& str) {
                std::istringstream is(str);
                cereal::XMLInputArchive archive(is);
                ResourceMetadata meta(TResource::getType(), TResource::loadMetadata(archive));
                return meta;
            });
    }
}
