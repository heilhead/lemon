#include <lemon/resource/ResourceManager.h>
#include <lemon/resource/types/MaterialResource.h>
#include <lemon/resource/types/TextureResource.h>
#include <lemon/resource/types/BundleResource.h>
#include <lemon/resource/types/ModelResource.h>

using namespace lemon::res;

static constexpr size_t kDefaultStoreSize = 1024;

ResourceManager::ResourceManager(std::filesystem::path&& rootPath)
    : store{kDefaultStoreSize}, factories{}, materialComposer{rootPath}
{
    root = rootPath;

    // Register base resource classes.
    registerClass<TextureResource>();
    registerClass<MaterialResource>();
    registerClass<BundleResource>();
    registerClass<ModelResource>();
}

std::filesystem::path
ResourceManager::resolvePath(const ResourceLocation& location) const
{
    return root / location.getFileName();
}

std::filesystem::path
ResourceManager::resolvePath(const std::string& relPath) const
{
    return root / relPath;
}

std::filesystem::path
ResourceManager::resolvePath(const std::filesystem::path& relPath) const
{
    return root / relPath;
}

ResourceContract*
ResourceManager::getContract(ResourceHandle handle) const
{
    return store.find(handle);
}

ResourceState
ResourceManager::getResourceState(ResourceHandle handle, ResourceObjectHandle object) const
{
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

ResourceContract::FutureType<ResourceInstance>
ResourceManager::loadResource(ResourceClassID id, const ResourceLocation& location, ResourceLifetime lifetime)
{
    return CPUTask(res_detail::coResourceFactory(id, location.getFileName(), lifetime));
}

bool
ResourceManager::unloadResource(const ResourceLocation& location)
{
    return unloadResource(location.handle);
}

bool
ResourceManager::unloadResource(ResourceHandle handle)
{
    auto state = getResourceState(handle);
    if (state != ResourceState::Ready) {
        // Resource is not available for unloading.
        return false;
    }

    auto* pContract = getContract(handle);
    auto* pResource = *pContract->getFuture<ResourceInstance>().value();

    if (pResource->getDependantCount() != 0) {
        // Other resources depend on it.
        return false;
    }

    // Okay to unload.
    std::vector<ResourceHandle> unusedResources;
    unusedResources.push_back(pResource->getHandle());

    // Traverse dependency tree and mark unused resources for unloading.
    pResource->detachDependencies(&unusedResources);

    for (auto& handle : unusedResources) {
        store.remove(handle);
    }

    return true;
}

std::optional<ResourceFactoryFn>
ResourceManager::getFactoryFn(ResourceClassID id) const
{
    auto result = factories.find(id);
    if (result != factories.end()) {
        return result->second;
    } else {
        return std::nullopt;
    }
}

FactoryResultType
lemon::res::res_detail::coResourceFactory(ResourceClassID classID, const std::string& refLocation,
                                          ResourceLifetime lifetime)
{
    auto factory = ResourceManager::get()->getFactoryFn(classID);
    if (!factory) {
        co_return tl::make_unexpected(ResourceLoadingError::FactoryMissing);
    }

    co_return co_await (*factory)(refLocation, lifetime);
}
