#pragma once

#include <cereal/archives/xml.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/types/unordered_map.hpp>
#include <cereal/types/vector.hpp>
#include <lemon/resource/common.h>
#include <filesystem>

namespace lemon::res {
    struct RawResourceReference {
        std::string location;
        ResourceType type;

        template<class Archive>
        void
        serialize(Archive& ar) {
            ar(CEREAL_NVP(location));
            ar(CEREAL_NVP(type));
        }
    };

    struct CommonResourceMetadata {
        std::vector<RawResourceReference> references;

        template<class Archive>
        void
        serialize(Archive& ar) {
            ar(CEREAL_NVP(references));
        }
    };

    struct ResourceMetadataBase {
        CommonResourceMetadata common;

        virtual ~ResourceMetadataBase() = default;
    };

    struct ResourceMetadataDescriptor {
        ResourceType type;
        std::unique_ptr<ResourceMetadataBase> data;
        std::filesystem::path fullPath;
        std::filesystem::path name;
    };

    struct ResourceMetadata {
        ResourceType type;
        std::unique_ptr<ResourceMetadataBase> data;
        std::filesystem::path fullPath;
        std::filesystem::path name;

        ResourceMetadata(ResourceMetadataDescriptor&& desc) {
            type = desc.type;
            data = std::move(desc.data);
            fullPath = std::move(desc.fullPath);
            name = std::move(desc.name);
        }

        template<typename T>
        [[nodiscard]] T*
        get() const {
            return dynamic_cast<T*>(data.get());
        }

        inline const std::vector<RawResourceReference>&
        getReferences() const {
            assert(data != nullptr);
            return get<ResourceMetadataBase>()->common.references;
        }
    };
} // namespace lemon::res