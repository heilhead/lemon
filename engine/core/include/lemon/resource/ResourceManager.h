#pragma once

//#include <lemon/scheduler.h>
#include <filesystem>
#include <folly/hash/Hash.h>

namespace lemon::res {
    static const char* kLocationObjectDelimiter = ":";

    using ResourceID = uint64_t;

    ResourceID getResourceID(std::string& file, std::optional<std::string>& object);

    struct ResourceLocation {
        ResourceID id;
        std::filesystem::path file;
        std::optional<std::string> object;

        explicit ResourceLocation(std::string& inLocation);
        ResourceLocation(std::string& inLocation, std::string&& inObject);
    };

    class ResourceManager {
    public:
        ResourceManager(std::filesystem::path&& rootPath);
        ~ResourceManager();

        static ResourceManager* get();

    public:
        std::filesystem::path locateFile(const ResourceLocation& location);

    private:
        std::filesystem::path root;
    };
}
