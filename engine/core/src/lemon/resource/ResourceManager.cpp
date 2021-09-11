#include <lemon/resource/ResourceManager.h>
#include <cassert>

using namespace lemon::res;

static ResourceManager* gInstance;

ResourceID lemon::res::getResourceID(std::string& file, std::optional<std::string>& object) {
    auto hash = folly::hash::fnva64(file);
    if (object) {
        hash = folly::hash::fnva64(*object, hash);
    }
    return hash;
}

ResourceLocation::ResourceLocation(std::string& inLocation) {
    size_t pos = inLocation.find(kLocationObjectDelimiter);
    if (pos != std::string::npos) {
        auto locSub = inLocation.substr(0, pos);

        file = inLocation.substr(0, pos);
        object = inLocation.substr(pos + 1);
        id = getResourceID(locSub, object);

        assert((*object).find(kLocationObjectDelimiter) == std::string::npos);
    } else {
        file = inLocation;
        object = std::nullopt;
        id = getResourceID(inLocation, object);
    }
}

ResourceLocation::ResourceLocation(std::string& inLocation, std::string&& inObject) {
    assert(inLocation.find(kLocationObjectDelimiter) == std::string::npos);
    assert(inObject.find(kLocationObjectDelimiter) == std::string::npos);

    file = inLocation;
    object = inObject;
    id = getResourceID(inLocation, object);
}

ResourceManager::ResourceManager(std::filesystem::path&& rootPath) {
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

std::filesystem::path ResourceManager::locateFile(const ResourceLocation& location) {
    return std::filesystem::path(root) / location.file;
}
