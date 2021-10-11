#include <lemon/resource/ResourceMetadata.h>
#include <lemon/resource/ResourceManager.h>
#include <lemon/resource/types/TextureResource.h>
#include <lemon/resource/types/texture/common.h>
#include <lemon/resource/types/texture/PNGDecoder.h>
#include <lemon/shared/filesystem.h>
#include <lemon/shared/logger.h>
#include <lemon/tasks/filesystem.h>
#include <lemon/shared/utils.h>

using namespace lemon::io;
using namespace lemon::res;
using namespace lemon::res::texture;
using namespace lemon::utils;

VoidTask<ResourceLoadingError>
loadTexture(const std::filesystem::path& filePath, TextureResource::Decoder decoder,
            InputColorChannels channels, uint8_t depth, ImageData& outData)
{
    auto bytes = co_await IOTask(coReadBinaryFile(filePath));
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

TextureResource::TextureResource()
{
    logger::log("TextureResource::TextureResource()");
}

TextureResource::~TextureResource()
{
    logger::log("TextureResource::~TextureResource()");
}

VoidTask<ResourceLoadingError>
TextureResource::load(ResourceMetadata&& meta)
{
    auto* pMeta = meta.get<Metadata>();
    GPUFormat = pMeta->GPUFormat;

    auto filePath = ResourceManager::get()->resolvePath(meta.name);
    auto loadingError = co_await loadTexture(filePath, pMeta->decoder, pMeta->inputChannels,
                                             pMeta->inputChannelDepth, imageData);
    if (loadingError) {
        co_return loadingError;
    }

    hash = lemon::hash(filePath.c_str());

    co_return {};
}
