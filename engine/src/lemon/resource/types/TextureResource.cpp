#include <lemon/resource/ResourceMetadata.h>
#include <lemon/resource/ResourceManager.h>
#include <lemon/resource/types/TextureResource.h>
#include <lemon/resource/types/texture/common.h>
#include <lemon/resource/types/texture/PNGDecoder.h>
#include <lemon/tasks/filesystem.h>

using namespace lemon::io;
using namespace lemon::res;
using namespace lemon::res::texture;
using namespace lemon::utils;

VoidTask<ResourceLoadingError>
loadTexture(const std::filesystem::path& filePath, TextureResource::Decoder decoder,
            InputColorChannels channels, uint8_t depth, ImageData& outData)
{
    auto bytes = co_await runIOThreadTask(coReadBinaryFile(filePath));
    if (!bytes) {
        co_return ResourceLoadingError::DataMissing;
    }

    switch (decoder) {
    case TextureResource::Decoder::PNG: {
        auto decodingError = decodePNG(bytes.value(), channels, depth, outData);

        if (decodingError) {
            co_return ResourceLoadingError::DataDecodingError;
        }

        break;
    }
    default:
        LEMON_TODO();
    }

    co_return {};
}

TextureResource::TextureResource() {}

TextureResource::~TextureResource() {}

VoidTask<ResourceLoadingError>
TextureResource::load(ResourceMetadata&& meta)
{
    OPTICK_EVENT();

    auto* pMeta = meta.get<Metadata>();
    auto filePath = ResourceManager::get()->resolvePath(meta.name);

    GPUFormat = pMeta->GPUFormat;
    bIsRenderTarget = pMeta->type == Type::RenderTarget;
    bHasImageData = pMeta->sourceDecoder != Decoder::None;
    mipLevelCount = pMeta->mipLevelCount;
    hash = lemon::hash(filePath.c_str());

    if (bIsRenderTarget) {
        width = pMeta->width;
        height = pMeta->height;

        if (width == 0 || height == 0) {
            logger::warn("render targets must have valid dimensions. texture path: ", filePath);
            co_return ResourceLoadingError::InitializationError;
        }
    }

    if (bHasImageData) {
        auto loadingError = co_await loadTexture(filePath, pMeta->sourceDecoder, pMeta->sourceChannels,
                                                 pMeta->sourceChannelDepth, imageData);
        if (loadingError) {
            logger::warn("texture decoding failed. texture path: ", filePath);
            co_return loadingError;
        }

        if (bIsRenderTarget && (width != imageData.width || height != imageData.height)) {
            logger::warn("render target dimensions must match source image data dimensions. texture path: ",
                         filePath);
            co_return ResourceLoadingError::DataDecodingError;
        }
    } else if (!bIsRenderTarget) {
        logger::warn("texture binding textures must have image data. texture path: ", filePath);
        co_return ResourceLoadingError::DataMissing;
    }

    co_return {};
}
