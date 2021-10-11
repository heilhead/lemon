#include <lemon/resource/types/texture/PNGDecoder.h>
#include <lemon/shared/logger.h>
#include <lodepng.h>

using namespace lemon;
using namespace lemon::res::texture;

std::optional<DecodingError>
lemon::res::texture::decodePNG(const UnalignedMemory& inBytes, InputColorChannels channels, uint8_t depth,
                               ImageData& outData)
{
    LodePNGColorType colorType;
    uint8_t stride;

    switch (channels) {
    case lemon::res::texture::InputColorChannels::R:
        colorType = LCT_GREY;
        stride = depth;
        break;
    case lemon::res::texture::InputColorChannels::RG:
        colorType = LCT_GREY_ALPHA;
        stride = depth * 2;
        break;
    case lemon::res::texture::InputColorChannels::RGB:
        colorType = LCT_RGB;
        stride = depth * 3;
        break;
    case lemon::res::texture::InputColorChannels::RGBA:
        colorType = LCT_RGBA;
        stride = depth * 4;
        break;
    default:
        LEMON_TODO();
    }

    // Convert bit depth to stride in bytes.
    outData.stride = stride / 8;

    uint8_t* pData = nullptr;
    auto error = lodepng_decode_memory(&pData, &outData.width, &outData.height, inBytes, inBytes.size(),
                                       colorType, depth);

    if (pData != nullptr) {
        size_t size = (size_t)outData.stride * outData.width * outData.height;
        outData.data.copy(pData, size);
        free(pData);
    }

    if (error) {
        return DecodingError::Unknown;
    }

    return {};
}
