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
        // BEGIN Resource traits
        /////////////////////////////////////////////////////////////////////////////////////

        struct Metadata : ResourceMetadataBase {
            std::unordered_map<std::string, int> shaderConfig;
            std::unordered_map<std::string, std::string> textures;

            template<class TArchive>
            void
            serialize(TArchive& ar) {
                ResourceMetadataBase::serialize(ar);

                LEMON_SERIALIZE(ar, shaderConfig);
                LEMON_SERIALIZE(ar, textures);
            }
        };

        LEMON_RESOURCE_TRAITS(MaterialResource);

        /////////////////////////////////////////////////////////////////////////////////////
        // END Resource traits
        /////////////////////////////////////////////////////////////////////////////////////

    public:
        MaterialResource();
        ~MaterialResource() override;

        VoidTask<ResourceLoadingError>
        load(ResourceMetadata& meta) override;
    };
} // namespace lemon::res
