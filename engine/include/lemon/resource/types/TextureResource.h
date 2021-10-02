#pragma once

#include <lemon/resource/common.h>
#include <lemon/resource/ResourceInstance.h>
#include <lemon/resource/ResourceMetadata.h>
#include <lemon/utils/utils.h>
#include <lemon/scheduler.h>
#include <dawn/webgpu_cpp.h>

namespace lemon::res {

    class TextureResource : public ResourceInstance {
    public:
        enum class Decoder { PNG, DDS };

        /////////////////////////////////////////////////////////////////////////////////////
        // BEGIN Resource traits
        /////////////////////////////////////////////////////////////////////////////////////

        struct Metadata : ResourceMetadataBase {
            Decoder decoder;
            wgpu::TextureFormat format;
            uint32_t width;
            uint32_t height;

            template<class TArchive>
            void
            serialize(TArchive& ar)
            {
                ResourceMetadataBase::serialize(ar);

                LEMON_SERIALIZE(ar, format);
                LEMON_SERIALIZE(ar, width);
                LEMON_SERIALIZE(ar, height);
            }
        };

        LEMON_RESOURCE_TRAITS(TextureResource);

        /////////////////////////////////////////////////////////////////////////////////////
        // END Resource traits
        /////////////////////////////////////////////////////////////////////////////////////

    public:
        TextureResource();
        ~TextureResource() override;

        VoidTask<ResourceLoadingError>
        load(ResourceMetadata&& meta) override;
    };
} // namespace lemon::res
