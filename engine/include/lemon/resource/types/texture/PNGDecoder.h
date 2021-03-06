#pragma once

#include <lemon/resource/types/texture/common.h>

namespace lemon::res::texture {
    std::optional<DecodingError>
    decodePNG(const UnalignedMemory& inBytes, InputColorChannels channels, uint8_t depth, ImageData& outData);
} // namespace lemon::res::texture
