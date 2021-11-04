#pragma once

#include <lemon/shared/logger.h>

namespace lemon {
    /// <summary>
    /// A raw pointer iterator. Using it is probably not a good idea.
    /// </summary>
    /// <typeparam name="T"></typeparam>
    template<typename T>
    class PtrRange {
        T* pBegin;
        T* pEnd;

    public:
        PtrRange(T* ptr, size_t length) : pBegin{ptr}, pEnd{ptr + length}
        {
            LEMON_ASSERT(pBegin <= pEnd);
        }

        T*
        begin() const
        {
            return pBegin;
        }

        T*
        end() const
        {
            return pEnd;
        }
    };

    template<typename T>
    PtrRange<T>
    range(T* ptr, size_t length)
    {
        return PtrRange<T>(ptr, length);
    }
} // namespace lemon
