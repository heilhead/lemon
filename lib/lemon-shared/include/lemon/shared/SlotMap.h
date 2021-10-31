#pragma once

#include <lemon/pch/folly.h>
#include <lemon/shared/common.h>
#include <lemon/shared/Memory.h>

namespace lemon {
    template<typename Tag>
    struct SlotMapHandle;

    namespace slotmap_detail {
        struct SlotMapKey;
        struct SlotMapDefaultHandleTag {
        };
    } // namespace slotmap_detail

    template<typename T>
    concept SlotMapDataType = std::move_constructible<T>;

    template<typename T, typename Tag>
    concept SlotMapHandleType = Base<T, SlotMapHandle<Tag>>;

    template<typename TTag = slotmap_detail::SlotMapDefaultHandleTag>
    struct SlotMapHandle {
    private:
        int32_t index{kInvalidIndex};
        uint32_t generation{0};

    public:
        using Tag = TTag;

        SlotMapHandle() = default;

        SlotMapHandle(int32_t index, uint32_t generation);

        int32_t
        getIndex() const;

        uint32_t
        getGeneration() const;

        bool
        operator==(const SlotMapHandle& other) const;
    };

    using SlotMapDefaultHandle = SlotMapHandle<slotmap_detail::SlotMapDefaultHandleTag>;

    template<SlotMapDataType TData, typename THandle = SlotMapDefaultHandle>
    class SlotMap {
        using Key = slotmap_detail::SlotMapKey;

        std::vector<MaybeUninit<TData>> data;
        std::vector<Key> keys;
        std::vector<uint32_t> keyLookup;
        size_t size;
        Key* pNextKey;

    public:
        using HandleTag = THandle::Tag;
        using Handle = THandle;

        SlotMap(size_t initialCapacity = 0);
        ~SlotMap();

        template<typename... TArgs>
        [[nodiscard]] Handle
        insert(TArgs&&... args);

        bool
        remove(Handle handle);

        bool
        remove(size_t index);

        Handle
        getHandle(size_t index) const;

        void
        clear();

        bool
        isValid(Handle handle);

        size_t
        getSize() const;

        size_t
        getCapacity() const;

        const TData&
        operator[](Handle handle) const;

        TData&
        operator[](Handle handle);

        const TData&
        operator[](size_t index) const;

        TData&
        operator[](size_t index);

        const TData&
        getData(size_t index) const;

        TData&
        getData(size_t index);

        const TData*
        getData(Handle handle) const;

        TData*
        getData(Handle handle);

    private:
        void
        reserve(size_t newCapacity);

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
    struct Name##Tag {                                                                                       \
    };                                                                                                       \
    using Name = ::lemon::SlotMapHandle<Name##Tag>

#include <lemon/shared/SlotMap.inl>
