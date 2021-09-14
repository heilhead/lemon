#pragma once

#include <lemon/resource/common.h>
#include <lemon/resource/ResourceInstance.h>
#include <lemon/resource/ResourceMetadata.h>
#include <lemon/utils/utils.h>
#include <lemon/scheduler.h>

namespace lemon::res {
    class MaterialResource : public ResourceInstance {
    public:
        /////////////////////////////////////////////////////////////////////////////////////
        // BEGIN Resource interface
        /////////////////////////////////////////////////////////////////////////////////////

        struct Metadata : ResourceMetadataBase {
            std::unordered_map<std::string, int> shaderConfig;
            std::unordered_map<std::string, std::string> textures;

            template<class Archive>
            void
            serialize(Archive& ar);
        };

        template<typename Archive>
        static std::unique_ptr<ResourceMetadataBase>
        loadMetadata(Archive& ar);

        template<typename Archive>
        static void
        saveMetadata(Archive& ar, const ResourceMetadata& data);

        /////////////////////////////////////////////////////////////////////////////////////
        // END Resource interface
        /////////////////////////////////////////////////////////////////////////////////////

    public:
        MaterialResource();
        ~MaterialResource() override;

        VoidTask<ResourceLoadingError>
        load(ResourceMetadata& meta) override;
    };
} // namespace lemon::res

#include <lemon/resource/types/MaterialResource.inl>
