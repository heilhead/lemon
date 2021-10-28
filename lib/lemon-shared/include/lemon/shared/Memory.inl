#pragma once

namespace lemon {
    template<Sized T>
    inline T*
    assumeInit(AlignedStorage<T>* mem)
    {
        return std::launder(reinterpret_cast<T*>(mem));
    }

    template<Sized T>
    inline const T*
    assumeInit(const AlignedStorage<T>* mem)
    {
        return std::launder(reinterpret_cast<const T*>(mem));
    }

    template<Sized TData>
    template<typename... TArgs>
    TData*
    MaybeUninit<TData>::init(TArgs&&... args)
    {
        return new (&data) TData(std::forward<TArgs>(args)...);
    }

    template<Sized TData>
    void
    MaybeUninit<TData>::destroy()
    {
        assumeInit()->~TData();
    }

    template<Sized TData>
    inline const TData&
    MaybeUninit<TData>::operator*() const
    {
        return *assumeInit();
    }

    template<Sized TData>
    inline TData&
    MaybeUninit<TData>::operator*()
    {
        return *assumeInit();
    }

    template<Sized TData>
    inline const TData*
    MaybeUninit<TData>::operator->() const
    {
        return assumeInit();
    }

    template<Sized TData>
    inline TData*
    MaybeUninit<TData>::operator->()
    {
        return assumeInit();
    }

    template<Sized TData>
    inline const TData*
    MaybeUninit<TData>::assumeInit() const
    {
        return ::assumeInit<TData>(&data);
    }

    template<Sized TData>
    inline TData*
    MaybeUninit<TData>::assumeInit()
    {
        return ::assumeInit<TData>(&data);
    }

    template<size_t Alignment, Sized TData>
    AlignedMemory<Alignment, TData>::AlignedMemory() : data{nullptr}, length{0}
    {
    }

    template<size_t Alignment, Sized TData>
    AlignedMemory<Alignment, TData>::AlignedMemory(size_t length) : AlignedMemory()
    {
        allocate(length);
    }

    template<size_t Alignment, Sized TData>
    AlignedMemory<Alignment, TData>::AlignedMemory(const AlignedMemory& other) : AlignedMemory()
    {
        copyAssign(other);
    }

    template<size_t Alignment, Sized TData>
    AlignedMemory<Alignment, TData>::AlignedMemory(AlignedMemory&& other) noexcept : AlignedMemory()
    {
        moveAssign(std::move(other));
    }

    template<size_t Alignment, Sized TData>
    const uint8_t*
    AlignedMemory<Alignment, TData>::operator*() const
    {
        return get();
    }

    template<size_t Alignment, Sized TData>
    AlignedMemory<Alignment, TData>::operator const uint8_t*() const
    {
        return get();
    }

    template<size_t Alignment, Sized TData>
    AlignedMemory<Alignment, TData>::operator uint8_t*()
    {
        return get();
    }

    template<size_t Alignment, Sized TData>
    AlignedMemory<Alignment, TData>&
    AlignedMemory<Alignment, TData>::operator=(const AlignedMemory& other)
    {
        if (this != &other) {
            copyAssign(other);
        }

        return *this;
    }

    template<size_t Alignment, Sized TData>
    AlignedMemory<Alignment, TData>&
    AlignedMemory<Alignment, TData>::operator=(AlignedMemory&& other) noexcept
    {
        if (this != &other) {
            moveAssign(std::move(other));
        }

        return *this;
    }

    template<size_t Alignment, Sized TData>
    size_t
    AlignedMemory<Alignment, TData>::size() const
    {
        return length;
    }

    template<size_t Alignment, Sized TData>
    void
    AlignedMemory<Alignment, TData>::copy(const uint8_t* pData, size_t size)
    {
        LEMON_ASSERT(size > 0);
        LEMON_ASSERT(pData != nullptr);

        allocate(size);
        auto err = memcpy_s(data.get(), size, pData, size);

        LEMON_ASSERT(!err);
    }

    template<size_t Alignment, Sized TData>
    void
    AlignedMemory<Alignment, TData>::allocate(size_t inLength)
    {
        if (length == inLength) {
            return;
        }

        if (inLength > 0) {
            LEMON_ASSERT(math::isAligned(inLength, Alignment),
                         "allocation size must be multiple of alignment");

            auto blocks = inLength / sizeof(DataBlock);

            data = std::unique_ptr<DataBlock[]>(new DataBlock[blocks]);
            length = inLength;
        } else {
            release();
        }
    }

    template<size_t Alignment, Sized TData>
    void
    AlignedMemory<Alignment, TData>::allocateItems(size_t capacity)
    {
        allocate(capacity * sizeof(TData));
    }

    template<size_t Alignment, Sized TData>
    inline void
    AlignedMemory<Alignment, TData>::release()
    {
        data = nullptr;
        length = 0;
    }

    template<size_t Alignment, Sized TData>
    template<typename TConcreteData>
    inline const TConcreteData*
    AlignedMemory<Alignment, TData>::get(size_t byteOffset) const
    {
        static_assert(alignof(DataBlock) % alignof(TConcreteData) == 0, "unaligned access");
        LEMON_ASSERT(math::isAligned(byteOffset, alignof(TConcreteData)), "unaligned access");
        LEMON_ASSERT(byteOffset >= 0 && byteOffset < length);
        return std::launder(
            reinterpret_cast<const TConcreteData*>(data.get() + (byteOffset / sizeof(DataBlock))));
    }

    template<size_t Alignment, Sized TData>
    template<typename TConcreteData>
    inline TConcreteData*
    AlignedMemory<Alignment, TData>::get(size_t byteOffset)
    {
        static_assert(alignof(DataBlock) % alignof(TConcreteData) == 0, "unaligned access");
        LEMON_ASSERT(math::isAligned(byteOffset, alignof(TConcreteData)), "unaligned access");
        LEMON_ASSERT(byteOffset >= 0 && byteOffset < length);
        return std::launder(reinterpret_cast<TConcreteData*>(data.get() + (byteOffset / sizeof(DataBlock))));
    }

    template<size_t Alignment, Sized TData>
    inline void
    AlignedMemory<Alignment, TData>::copyAssign(const AlignedMemory& other)
    {
        if (other.length > 0) {
            allocate(other.length);
            auto err = memcpy_s(data.get(), length, other.data.get(), length);
            LEMON_ASSERT(!err);
        } else {
            release();
        }
    }

    template<size_t Alignment, Sized TData>
    inline void
    AlignedMemory<Alignment, TData>::moveAssign(AlignedMemory&& other)
    {
        if (other.length > 0) {
            data = std::move(other.data);
            length = other.length;
            other.length = 0;
        } else {
            release();
        }
    }
} // namespace lemon
