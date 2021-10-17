#pragma once

namespace lemon::res::texture {
    enum class DecodingError {
        Unknown,
    };

    enum class InputColorChannels { R, RG, RGB, RGBA };

    struct ImageData {
        UnalignedMemory data;
        uint32_t width;
        uint32_t height;
        uint8_t stride;
    };
} // namespace lemon::res::texture
