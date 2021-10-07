#pragma once

#include <cstdint>
#include <lemon/shared/DataBuffer.h>

namespace lemon::render {
    class BufferBinding {
        static const uint32_t kMaxOffsets = 8;

        uint32_t baseOffset;
        uint32_t offsetCount;
        uint32_t offsets[kMaxOffsets];
        HeapBuffer data;

    public:
        BufferBinding();
    };
} // namespace lemon::render