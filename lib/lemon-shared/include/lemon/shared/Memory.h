#pragma once

#include <cstdint>
#include <cstdlib>
#include <memory>
#include <lemon/shared/logger.h>
#include <lemon/shared/math.h>

namespace lemon {
    template<typename T>
    concept Sized = sizeof(T) > 0;

    namespace {
        inline void*
        alignedAlloc(size_t size, size_t alignment)
        {
            LEMON_ASSERT(size > 0);
            LEMON_ASSERT(math::isPowerOfTwo(alignment));

            void* ptr;

#if defined(_MSC_VER)
            ptr = _aligned_malloc(size, alignment);
#else
            ptr = std::aligned_alloc(alignment, size);
#endif

            LEMON_ASSERT(math::isPtrAligned(ptr, alignment));

            return ptr;
        }

        inline void
        alignedFree(void* ptr)
        {
#if defined(_MSC_VER)
            _aligned_free(ptr);
#else
            std::free(ptr);
#endif
        }

        struct AlignedDeleter {
            inline void
            operator()(void* ptr) const
            {
                alignedFree(ptr);
            }
        };
    } // namespace

    /// <summary>
    /// Heap-allocated aligned memory. Sanity checks aligned access and allocation.
    /// </summary>
    template<size_t Alignment, typename TItem = uint8_t>
    class AlignedMemory {
        std::unique_ptr<uint8_t[], AlignedDeleter> data;
        size_t length;

    public:
        AlignedMemory() : data{nullptr}, length{0} {}

        explicit AlignedMemory(size_t length) : AlignedMemory()
        {
            allocate(length);
        }

        AlignedMemory(const AlignedMemory& other) : AlignedMemory()
        {
            copyAssign(other);
        }

        AlignedMemory(AlignedMemory&& other) noexcept : AlignedMemory()
        {
            moveAssign(std::move(other));
        }

        const uint8_t*
        operator*() const
        {
            return data.get();
        }

        operator uint8_t*() const
        {
            return data.get();
        }

        AlignedMemory&
        operator=(const AlignedMemory& other)
        {
            if (this != &other) {
                copyAssign(other);
            }

            return *this;
        }

        AlignedMemory&
        operator=(AlignedMemory&& other) noexcept
        {
            if (this != &other) {
                moveAssign(std::move(other));
            }

            return *this;
        }

        size_t
        size() const
        {
            return length;
        }

        void
        copy(const uint8_t* pData, size_t size)
        {
            LEMON_ASSERT(size > 0);
            LEMON_ASSERT(pData != nullptr);

            allocate(size);
            auto err = memcpy_s(data.get(), size, pData, size);

            LEMON_ASSERT(!err);
        }

        void
        allocate(size_t inLength)
        {
            if (length == inLength) {
                return;
            }

            if (inLength > 0) {
                LEMON_ASSERT(math::isAligned(inLength, Alignment),
                             "allocation size must be multiple of alignment");

                data = std::unique_ptr<uint8_t[], AlignedDeleter>(
                    reinterpret_cast<uint8_t*>(alignedAlloc(inLength, Alignment)), AlignedDeleter());
                length = inLength;
            } else {
                release();
            }
        }

        void
        allocateItems(size_t capacity)
        {
            allocate(capacity * sizeof(TItem));
        }

        inline void
        release()
        {
            data = nullptr;
            length = 0;
        }

        template<typename TData = uint8_t>
        inline const TData*
        get(size_t byteOffset = 0) const
        {
            LEMON_ASSERT(math::isAligned(byteOffset, alignof(TData)), "unaligned access");
            LEMON_ASSERT(byteOffset >= 0 && byteOffset < length);
            return reinterpret_cast<const TData*>(data.get() + byteOffset);
        }

        template<typename TData = uint8_t>
        inline TData*
        get(size_t byteOffset = 0)
        {
            LEMON_ASSERT(math::isAligned(byteOffset, alignof(TData)), "unaligned access");
            LEMON_ASSERT(byteOffset >= 0 && byteOffset < length);
            return reinterpret_cast<TData*>(data.get() + byteOffset);
        }

        inline const TItem*
        getItem(size_t index) const
        {
            return get<TItem>(index * sizeof(TItem));
        }

        inline TItem*
        getItem(size_t index)
        {
            return get<TItem>(index * sizeof(TItem));
        }

        inline const TItem*
        operator[](size_t index) const
        {
            return getItem(index);
        }

        inline TItem*
        operator[](size_t index)
        {
            return getItem(index);
        }

    private:
        inline void
        copyAssign(const AlignedMemory& other)
        {
            if (other.length > 0) {
                allocate(other.length);
                auto err = memcpy_s(data.get(), length, other.data.get(), length);
                LEMON_ASSERT(!err);
            } else {
                release();
            }
        }

        inline void
        moveAssign(AlignedMemory&& other)
        {
            if (other.length > 0) {
                data = std::move(other.data);
                length = other.length;
                other.length = 0;
            } else {
                release();
            }
        }
    };

    using UnalignedMemory = AlignedMemory<1>;

    template<Sized T>
    using HeapArray = AlignedMemory<alignof(T), T>;

    template<Sized T>
    using MaybeUninit = std::aligned_storage<sizeof(T), alignof(T)>::type;

    template<Sized T>
    inline T*
    assumeInit(MaybeUninit<T>* mem)
    {
        LEMON_ASSERT(math::isPtrAligned(mem, alignof(T)));
        return std::launder(reinterpret_cast<T*>(mem));
    }

    template<Sized T>
    inline const T*
    assumeInit(const MaybeUninit<T>* mem)
    {
        LEMON_ASSERT(math::isPtrAligned(mem, alignof(T)));
        return std::launder(reinterpret_cast<const T*>(mem));
    }
} // namespace lemon
