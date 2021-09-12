#include <lemon/resource/ResourceLocation.h>

using namespace lemon::res;

ResourceLocation::ResourceLocation(std::string& inLocation) {
    size_t pos = inLocation.find(kLocationObjectDelimiter);
    if (pos != std::string::npos) {
        auto locSub = inLocation.substr(0, pos);

        file = inLocation.substr(0, pos);
        object = inLocation.substr(pos + 1);
        id = getResourceID(locSub);

        assert((*object).find(kLocationObjectDelimiter) == std::string::npos);
    } else {
        file = inLocation;
        object = std::nullopt;
        id = getResourceID(inLocation);
    }
}

ResourceLocation::ResourceLocation(std::string& inLocation, std::string&& inObject) {
    assert(inLocation.find(kLocationObjectDelimiter) == std::string::npos);
    assert(inObject.find(kLocationObjectDelimiter) == std::string::npos);

    file = inLocation;
    object = inObject;
    id = getResourceID(inLocation);
}
