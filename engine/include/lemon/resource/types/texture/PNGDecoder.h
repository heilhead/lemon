#pragma once

#include <optional>
#include <lemon/resource/types/texture/common.h>

namespace lemon::res::texture {
    std::optional<DecodingError>
    decodePNG(const HeapBuffer& inBytes, InputColorChannels channels, uint8_t depth, ImageData& outData);
} // namespace lemon::res::texture