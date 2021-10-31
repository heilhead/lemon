#pragma once

#include <cstdint>
#include <cstdlib>
#include <memory>
#include <lemon/shared/logger.h>
#include <lemon/shared/math.h>
#include <lemon/shared/concepts.h>

namespace lemon {
    template<Sized T>
    using AlignedStorage = std::aligned_storage<sizeof(T), alignof(T)>::type;

    template<Sized T>
    T*
    assumeInit(AlignedStorage<T>* mem);

    template<Sized T>
    const T*
    assumeInit(const AlignedStorage<T>* mem);

    /// <summary>
    /// Aligned memory block intended to be manually initialized and destroyed.
    ///
    /// Note: Will not call the destructor even if the memory was initialized. Both `init()` and `destroy()`
    /// need to be called manually.
    /// </summary>
    template<Sized TData>
    struct MaybeUninit {
        using Data = TData;

    private:
        AlignedStorage<TData> data;

    public:
        MaybeUninit() = default;

        template<typename... TArgs>
        TData*
        init(TArgs&&... args);

        void
        destroy();

        const TData&
        operator*() const;

        TData&
        operator*();

        const TData*
        operator->() const;

        TData*
        operator->();

        const TData*
        assumeInit() const;

        TData*
        assumeInit();
    };

    /// <summary>
    /// Dynamic heap-allocated and aligned memory buffer. Provides type conversion methods and ensures aligned
    /// access.
    /// </summary>
    template<size_t Alignment, Sized TData = uint8_t>
    class AlignedMemory {
        using DataBlock = std::aligned_storage<Alignment, Alignment>::type;

        std::unique_ptr<DataBlock[]> data;
        size_t length;

    public:
        AlignedMemory();

        explicit AlignedMemory(size_t length);

        AlignedMemory(const AlignedMemory& other);

        AlignedMemory(AlignedMemory&& other) noexcept;

        const uint8_t*
        operator*() const;

        operator const uint8_t*() const;

        operator uint8_t*();

        AlignedMemory&
        operator=(const AlignedMemory& other);

        AlignedMemory&
        operator=(AlignedMemory&& other) noexcept;

        size_t
        size() const;

        void
        copy(const uint8_t* pData, size_t size);

        void
        allocate(size_t inLength);

        void
        allocateItems(size_t capacity);

        void
        release();

        template<typename TConcreteData = TData>
        const TConcreteData*
        get(size_t byteOffset = 0) const;

        template<typename TConcreteData = TData>
        TConcreteData*
        get(size_t byteOffset = 0);

    private:
        void
        copyAssign(const AlignedMemory& other);

        void
        moveAssign(AlignedMemory&& other);
    };

    using UnalignedMemory = AlignedMemory<1>;
} // namespace lemon

#include <lemon/shared/Memory.inl>
