#include <lemon/resource/ResourceManager.h>
#include <cassert>
#include <lemon/resource/ResourceLocation.h>

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
