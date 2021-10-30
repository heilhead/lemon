#pragma once

#include <lemon/pch/folly.h>
#include <lemon/shared/common.h>
#include <lemon/shared/Memory.h>

namespace lemon {
    struct SlotMapHandle;

    namespace slotmap_detail {
        struct SlotMapKey;
    }

    template<typename T>
    concept SlotMapDataType = std::move_constructible<T>;

    template<typename T>
    concept SlotMapHandleType = Base<T, SlotMapHandle> || std::is_same_v<T, SlotMapHandle>;

    struct SlotMapHandle {
    private:
        int32_t index{kInvalidIndex};
        uint32_t generation{0};

    public:
        SlotMapHandle() = default;

        SlotMapHandle(int32_t index, uint32_t generation);

        int32_t
        getIndex() const;

        uint32_t
        getGeneration() const;

        bool
        operator==(const SlotMapHandle& other) const;
    };

    template<SlotMapDataType TData, uint32_t Capacity, SlotMapHandleType THandle = SlotMapHandle>
    class SlotMap {
        using Key = slotmap_detail::SlotMapKey;

        MaybeUninit<TData> data[Capacity];
        Key keys[Capacity];
        size_t size;
        Key* pNextKey;
        uint32_t keyLookup[Capacity];

    public:
        SlotMap();
        ~SlotMap();

        template<typename... TArgs>
        [[nodiscard]] THandle
        insert(TArgs&&... args);

        bool
        remove(THandle handle);

        bool
        remove(size_t index);

        THandle
        getHandle(size_t index) const;

        void
        clear();

        bool
        isValid(THandle handle);

        size_t
        getSize() const;

        size_t
        getCapacity() const;

        const TData&
        operator[](THandle handle) const;

        TData&
        operator[](THandle handle);

        const TData&
        operator[](size_t index) const;

        TData&
        operator[](size_t index);

        const TData&
        getData(size_t index) const;

        TData&
        getData(size_t index);

        const TData*
        getData(THandle handle) const;

        TData*
        getData(THandle handle);

    private:
        void
        removeImpl(size_t currDataIndex);

        size_t
        popKey();

        void
        pushKey(size_t keyIndex);

        inline size_t
        getKeyIndex(const Key* ptr) const;
    };

    namespace slotmap_detail {
        struct SlotMapKey {
            folly::PackedSyncPtr<SlotMapKey> next;
            uint32_t dataIndex;
            uint32_t generation;

            SlotMapKey();

            void
            setNext(SlotMapKey* pNext);

            SlotMapKey*
            getNext() const;

            bool
            isUsed() const;

            void
            setUsed(bool bUsed);

            void
            bump();
        };
    } // namespace slotmap_detail
} // namespace lemon

#define LEMON_SLOT_MAP_HANDLE(Name)                                                                          \
    struct Name : ::lemon::SlotMapHandle {                                                                   \
        Name() = default;                                                                                    \
        Name(int32_t index, uint32_t generation) : SlotMapHandle(index, generation) {}                       \
    }

#include <lemon/shared/SlotMap.inl>
