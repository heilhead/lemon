#include <lemon/resource/ResourceManager.h>
#include <cassert>
#include <lemon/resource/types/MaterialResource.h>

using namespace lemon::res;

static ResourceManager* gInstance;
static constexpr size_t kDefaultStoreSize = 1024;

ResourceManager::ResourceManager(std::filesystem::path&& rootPath)
    :store { kDefaultStoreSize } {
    assert(gInstance == nullptr);
    gInstance = this;
    root = rootPath;
}

ResourceManager::~ResourceManager() {
    gInstance = nullptr;
}

ResourceManager* ResourceManager::get() {
    return gInstance;
}

std::filesystem::path ResourceManager::resolvePath(const ResourceLocation& location) {
    return std::filesystem::path(root) / location.file;
}

ResourceContract* ResourceManager::getContract(ResourceHandle handle) {
    return store.find(handle);
}

ResourceState ResourceManager::getResourceState(ResourceHandle handle, ResourceObjectHandle object) {
    ResourceContract* pContract = getContract(handle);
    if (pContract == nullptr) {
        return ResourceState::NotLoaded;
    }

    auto& promise = pContract->getPromise<ResourceInstance>();
    if (!promise.isFulfilled()) {
        return ResourceState::Loading;
    }

    auto& future = pContract->getFuture<ResourceInstance>();
    auto& resolution = future.value();
    if (!resolution) {
        return ResourceState::LoadingError;
    }

    if (object.isValid()) {
        ResourceInstance* pResource = *resolution;
        if (pResource->getObject(object) == nullptr) {
            return ResourceState::ObjectNotFound;
        }
    }

    return ResourceState::Ready;
}

lemon::res::detail::FactoryResultType
lemon::res::detail::resourceFactory(ResourceManager& manager, ResourceType type, ResourceLocation location) {
    // @TODO std::move location or pass string reference and construct it here
    tprint("resourceFactory: type=", (int)type, " location.file=", location.file);

    // @TODO is there a cleaner way to implement this factory?
    switch (type) {
    case ResourceType::Material:
        co_return (co_await loadImpl<MaterialResource>(manager, location))
            .map([](auto* v) { return reinterpret_cast<ResourceInstance*>(v); });
    }

    assert(false);
}