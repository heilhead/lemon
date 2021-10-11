#pragma once

#include <lemon/shared/logger.h>
#include <cstdint>
#include <concepts>

namespace lemon::math {
    inline bool
    isPowerOfTwo(std::integral auto value)
    {
        return value > 0 && ((value & (value - 1)) == 0);
    }

    template<std::integral T>
    inline T
    alignUp(T value, T alignment)
    {
        LEMON_ASSERT(isPowerOfTwo(alignment));
        return (value + alignment - 1) & ~(alignment - 1);
    }

    template<std::integral T>
    inline T
    alignDown(T value, T alignment)
    {
        LEMON_ASSERT(isPowerOfTwo(alignment));
        return value & ~(alignment - 1);
    }

    template<std::integral T>
    inline bool
    isAligned(T value, T alignment)
    {
        LEMON_ASSERT(isPowerOfTwo(alignment));
        return (value & (alignment - 1)) == 0;
    }

    inline bool
    isPtrAligned(const void* ptr, size_t alignment)
    {
        LEMON_ASSERT(isPowerOfTwo(alignment));
        return isAligned(reinterpret_cast<size_t>(ptr), alignment);
    }
} // namespace lemon::math
