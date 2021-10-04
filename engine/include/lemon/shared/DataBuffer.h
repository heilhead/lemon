#pragma once

#include <lemon/shared/assert.h>
#include <cstdint>

namespace lemon {
    class HeapAllocator {
    public:
        static uint8_t*
        allocate(size_t length)
        {
            return new uint8_t[length];
        }

        static void
        release(const uint8_t* ptr)
        {
            delete[] ptr;
        }
    };

    template<typename T>
    class DataBuffer {
        using Allocator = T;

    public:
        DataBuffer() : data{nullptr}, length{0} {}

        explicit DataBuffer(size_t length) noexcept : DataBuffer()
        {
            allocate(length);
        }

        DataBuffer(DataBuffer&& other) noexcept : DataBuffer()
        {
            *this = std::move(other);
        }

        DataBuffer(const DataBuffer& other) noexcept : DataBuffer()
        {
            *this = other;
        }

        ~DataBuffer()
        {
            if (data != nullptr) {
                release();
            }
        }

    private:
        uint8_t* data;
        size_t length;

    public:
        uint8_t*
        operator*()
        {
            return data;
        }

        DataBuffer&
        operator=(const DataBuffer& other)
        {
            if (data != nullptr) {
                release();
            }

            allocate(other.length);
            auto err = memcpy_s(data, length, other.data, length);
            LEMON_ASSERT(!err);

            return *this;
        }

        DataBuffer&
        operator=(DataBuffer&& other) noexcept
        {
            if (data != nullptr) {
                release();
            }

            data = other.data;
            length = other.length;
            other.data = nullptr;
            other.length = 0;

            return *this;
        }

        size_t
        size() const
        {
            return length;
        }

        void
        allocate(size_t inLength)
        {
            if (data != nullptr) {
                release();
            }

            data = Allocator::allocate(inLength);
            length = inLength;
            LEMON_ASSERT(data != nullptr);
        }

        void
        release()
        {
            LEMON_ASSERT(data != nullptr);
            Allocator::release(data);
            data = nullptr;
            length = 0;
        }

        template<typename U>
        U*
        get() const
        {
            return (U*)data;
        }
    };

    using HeapBuffer = DataBuffer<HeapAllocator>;

    template<size_t N>
    class InlineBuffer {
    public:
        InlineBuffer() : data{} {}

    private:
        uint8_t data[N];

    public:
        template<typename T>
        T*
        get()
        {
            return (T*)&data;
        }
    };
} // namespace lemon