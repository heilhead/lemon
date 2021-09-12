#pragma once

#include <lemon/resource/common.h>
#include <cereal/archives/xml.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/unordered_map.hpp>
#include <cereal/types/memory.hpp>

namespace lemon::res {
    struct RawResourceReference {
        std::string location;
        ResourceType type;

        template<class Archive>
        void serialize(Archive& ar) {
            ar(CEREAL_NVP(location));
            ar(CEREAL_NVP(type));
        }
    };

    struct CommonResourceMetadata {
        std::vector<RawResourceReference> references;

        template<class Archive>
        void serialize(Archive& ar) {
            ar(CEREAL_NVP(references));
        }
    };

    struct ResourceMetadataBase {
        CommonResourceMetadata common;

        virtual ~ResourceMetadataBase() = default;
    };

    struct ResourceMetadata {
        ResourceType type;
        std::unique_ptr<ResourceMetadataBase> data;

        ResourceMetadata(ResourceType inType, std::unique_ptr<ResourceMetadataBase>&& inData) {
            type = inType;
            data = std::move(inData);
        }

        template<typename T>
        [[nodiscard]] T* get() const {
            return dynamic_cast<T*>(data.get());
        }

        inline const std::vector<RawResourceReference>& getReferences() const {
            assert(data != nullptr);
            return get<ResourceMetadataBase>()->common.references;
        }
    };
}