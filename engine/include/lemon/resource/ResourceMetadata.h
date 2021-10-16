#pragma once

#include <lemon/serialization.h>
#include <lemon/resource/common.h>
#include <lemon/shared/logger.h>
#include <lemon/shared/NonCopyable.h>
#include <filesystem>

namespace lemon::res {
    struct RawResourceReference {
        std::string location;
        ResourceClassID type;

        template<class Archive>
        void
        serialize(Archive& ar)
        {
            LEMON_SERIALIZE(ar, location);
            LEMON_SERIALIZE(ar, type);
        }
    };

    struct CommonResourceMetadata {
        std::vector<RawResourceReference> references;

        template<class Archive>
        void
        serialize(Archive& ar)
        {
            LEMON_SERIALIZE(ar, references);
        }
    };

    struct ResourceMetadataBase : private NonMovable {
        CommonResourceMetadata common;

        virtual ~ResourceMetadataBase() = default;

        template<class TArchive>
        void
        serialize(TArchive& ar)
        {
            LEMON_SERIALIZE(ar, common);
        }

        template<class TArchive>
        constexpr bool
        isSaving(TArchive& ar) const
        {
            return serialization::isSaving(ar);
        }

        template<class TArchive>
        constexpr bool
        isLoading(TArchive& ar) const
        {
            return serialization::isLoading(ar);
        }

        template<class TResource>
        void
        addReference(const std::string& refPath)
        {
            common.references.emplace_back(RawResourceReference{refPath, getClassID<TResource>()});
        }
    };

    struct ResourceMetadataDescriptor {
        ResourceClassID type;
        std::unique_ptr<ResourceMetadataBase> data;
        std::filesystem::path fullPath;
        std::filesystem::path name;
    };

    struct ResourceMetadata {
        ResourceClassID type;
        std::unique_ptr<ResourceMetadataBase> data;
        std::filesystem::path fullPath;
        std::filesystem::path name;

        ResourceMetadata(ResourceMetadataDescriptor&& desc)
        {
            type = desc.type;
            data = std::move(desc.data);
            fullPath = std::move(desc.fullPath);
            name = std::move(desc.name);
        }

        template<typename T>
        [[nodiscard]] T*
        get() const
        {
            return dynamic_cast<T*>(data.get());
        }

        inline const std::vector<RawResourceReference>&
        getReferences() const
        {
            LEMON_ASSERT(data != nullptr);
            return get<ResourceMetadataBase>()->common.references;
        }
    };
} // namespace lemon::res
