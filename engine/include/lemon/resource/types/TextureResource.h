#pragma once

#include <lemon/resource/common.h>
#include <lemon/resource/ResourceInstance.h>
#include <lemon/resource/ResourceMetadata.h>
#include <lemon/resource/types/texture/common.h>
#include <lemon/shared/utils.h>
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
            texture::InputColorChannels inputChannels;
            uint8_t inputChannelDepth;

            // TODO: Can one texture have multiple variants, e.g. SRGB and non-SRGB?
            // If so, it may make sense to expose several subojects for each format.
            wgpu::TextureFormat GPUFormat;

            template<class TArchive>
            void
            serialize(TArchive& ar)
            {
                ResourceMetadataBase::serialize(ar);

                LEMON_SERIALIZE(ar, decoder);
                LEMON_SERIALIZE(ar, inputChannels);
                LEMON_SERIALIZE(ar, inputChannelDepth);
                LEMON_SERIALIZE(ar, GPUFormat);
            }
        };

        LEMON_RESOURCE_TRAITS(TextureResource);

        /////////////////////////////////////////////////////////////////////////////////////
        // END Resource traits
        /////////////////////////////////////////////////////////////////////////////////////

    private:
        texture::ImageData imageData;
        wgpu::TextureFormat GPUFormat;
        uint64_t hash = 0;

    public:
        TextureResource();
        ~TextureResource() override;

        VoidTask<ResourceLoadingError>
        load(ResourceMetadata&& meta) override;

        inline const texture::ImageData&
        getImageData() const
        {
            return imageData;
        }

        inline const wgpu::TextureFormat&
        getGPUFormat() const
        {
            return GPUFormat;
        }

        // TODO: Currently, texture hash is based on the file path, completely ignoring other parameters,
        // assuming there won't be different variations of the same texture (e.g. srgb/non-srgb versions).
        // This will need to be improved in future.
        inline const uint64_t
        getHash() const
        {
            return hash;
        }
    };
} // namespace lemon::res

template<>
struct folly::hasher<lemon::res::TextureResource> {
    using folly_is_avalanching = std::true_type;

    size_t
    operator()(const lemon::res::TextureResource& data) const
    {
        return data.getHash();
    }
};