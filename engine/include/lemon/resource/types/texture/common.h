#pragma once

#include <lemon/shared/DataBuffer.h>

namespace lemon::res::texture {
    enum class DecodingError {
        Unknown,
    };

    enum class InputColorChannels { R, RG, RGB, RGBA };

    struct ImageData {
        HeapBuffer data;
        uint32_t width;
        uint32_t height;
        uint8_t stride;
    };
} // namespace lemon::res::texture