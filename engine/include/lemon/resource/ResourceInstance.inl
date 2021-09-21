#pragma once

#include <lemon/resource/ResourceLocation.h>

// Implement default metadata serialization methods with the specified tag.
#define LEMON_RESOURCE_TRAITS(tag)                                                                           \
    template<typename TArchive>                                                                              \
    static std::unique_ptr<ResourceMetadataBase> loadMetadata(TArchive& ar) {                                \
        std::unique_ptr<Metadata> meta = std::make_unique<Metadata>();                                       \
        LEMON_SERIALIZE_NVP(ar, tag, *meta);                                                                 \
        return meta;                                                                                         \
    }                                                                                                        \
    template<typename TArchive>                                                                              \
    static void saveMetadata(TArchive& ar, const ResourceMetadata& data) {                                   \
        auto* meta = data.get<Metadata>();                                                                   \
        LEMON_SERIALIZE_NVP(ar, tag, *meta);                                                                 \
    }

namespace lemon::res {
    template<class TObject>
    const TObject*
    ResourceInstance::getObject(ResourceObjectHandle handle) const {
        if (!handle.isValid()) {
            return nullptr;
        }

        for (auto* pObj : objects) {
            if (pObj->getHandle() == handle) {
                return dynamic_cast<TObject*>(pObj);
            }
        }

        return nullptr;
    }

    template<class TObject>
    const TObject*
    ResourceInstance::getObject(const ResourceLocation& location) const {
        if (location.handle != handle) {
            // Location references a different resource;
            return nullptr;
        }

        return getObject<TObject>(location.object);
    }

    template<class TObject, typename... Args>
    void
    ResourceInstance::createObject(ResourceObjectHandle handle, Args&&... args) {
        assert(getObject(handle) == nullptr);
        auto pObj = new TObject(std::forward<Args>(args)...);
        pObj->setHandle(handle);
        objects.emplace_back(pObj);
    }
} // namespace lemon::res