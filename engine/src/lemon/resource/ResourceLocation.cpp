#include <lemon/resource/ResourceLocation.h>
#include <lemon/shared/logger.h>

using namespace lemon::res;

ResourceLocation::ResourceLocation(const std::string& inLocation)
{
    size_t pos = inLocation.find(kLocationObjectDelimiter);
    if (pos != std::string::npos) {
        auto sFile = inLocation.substr(0, pos);
        auto sObject = inLocation.substr(pos + 1);

        handle = ResourceHandle(createHash(sFile));
        object = ResourceObjectHandle(createHash(sObject));

        LEMON_ASSERT((sObject).find(kLocationObjectDelimiter) == std::string::npos);
    } else {
        handle = ResourceHandle(createHash(inLocation));
        object = ResourceObjectHandle::InvalidHandle;
    }
}

ResourceLocation::ResourceLocation(const std::string& inLocation, std::string& inObject)
{
    LEMON_ASSERT(inLocation.find(kLocationObjectDelimiter) == std::string::npos);
    LEMON_ASSERT(inObject.find(kLocationObjectDelimiter) == std::string::npos);

    handle = ResourceHandle(createHash(inLocation));
    object = ResourceObjectHandle(createHash(inObject));
}
