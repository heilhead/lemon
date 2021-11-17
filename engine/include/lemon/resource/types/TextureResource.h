#pragma once

#include <lemon/resource/common.h>
#include <lemon/resource/ResourceInstance.h>
#include <lemon/resource/ResourceMetadata.h>
#include <lemon/resource/types/texture/common.h>
#include <lemon/scheduler.h>

namespace lemon::res {
    class TextureResource : public ResourceInstance {
    public:
        enum class Type { Texture, RenderTarget };
        enum class Decoder { None, PNG, DDS };

        /////////////////////////////////////////////////////////////////////////////////////
        // BEGIN Resource traits
        /////////////////////////////////////////////////////////////////////////////////////

        struct Metadata : ResourceMetadataBase {
            Type type = Type::Texture;
            wgpu::TextureFormat GPUFormat = wgpu::TextureFormat::RGBA8Unorm;
            uint32_t mipLevelCount = 1;
            uint32_t width = 0;
            uint32_t height = 0;
            Decoder sourceDecoder;
            texture::InputColorChannels sourceChannels = texture::InputColorChannels::RGBA;
            uint8_t sourceChannelDepth = 8;

            template<class TArchive>
            void
            serialize(TArchive& ar)
            {
                ResourceMetadataBase::serialize(ar);

                LEMON_SERIALIZE(ar, type);
                LEMON_SERIALIZE(ar, GPUFormat);
                LEMON_SERIALIZE(ar, mipLevelCount);
                LEMON_SERIALIZE(ar, sourceDecoder);

                // `Decoder::None` means it's a render target, so we must define dimensions.
                if (sourceDecoder == Decoder::None) {
                    LEMON_SERIALIZE(ar, width);
                    LEMON_SERIALIZE(ar, height);
                } else {
                    LEMON_SERIALIZE(ar, sourceChannels);
                    LEMON_SERIALIZE(ar, sourceChannelDepth);
                }
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
        uint32_t width = 0;
        uint32_t height = 0;
        uint32_t mipLevelCount = 0;
        bool bIsRenderTarget = false;
        bool bHasImageData = false;

    public:
        TextureResource();
        ~TextureResource() override;

        VoidTask<ResourceLoadingError>
        load(ResourceMetadata&& meta) override;

        inline bool
        hasImageData() const
        {
            return bHasImageData;
        }

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

        inline uint32_t
        getMipLevelCount() const
        {
            return mipLevelCount;
        }

        inline bool
        isRenderTarget() const
        {
            return bIsRenderTarget;
        }

        inline std::pair<uint32_t, uint32_t>
        getRenderTargetDimensions() const
        {
            return {width, height};
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
