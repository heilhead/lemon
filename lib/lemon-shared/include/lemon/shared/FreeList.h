#pragma once

#include <lemon/shared/logger.h>
#include <lemon/shared/Memory.h>

namespace lemon {
    namespace freelist_detail {
        template<typename TData, size_t Capacity, bool bHeapAllocated>
        struct Storage;
    }

    template<typename TData, size_t Capacity, bool bHeapAllocated = false>
    class FreeList {
        union DataWrapper {
            DataWrapper* pNext;
            MaybeUninit<TData> data;
        };

        freelist_detail::Storage<DataWrapper, Capacity, bHeapAllocated> storage;
        DataWrapper* pHead;
        size_t freeCount;

    public:
        FreeList();

        ~FreeList() noexcept;

        template<typename... TArgs>
        size_t
        insert(TArgs&&... args);

        void
        remove(size_t index);

        void
        remove(TData* pItem);

        size_t
        getFreeCount();

        size_t
        getCapacity();

        size_t
        getIndex(const TData* ptr) const;

        TData&
        operator[](size_t index);

        const TData&
        operator[](size_t index) const;

    private:
        DataWrapper*
        getItem(size_t index);

        const DataWrapper*
        getItem(size_t index) const;

        const TData*
        getData(size_t index) const;

        TData*
        getData(size_t index);
    };

    namespace freelist_detail {
        template<typename TData, size_t Capacity, bool bHeapAllocated>
        struct Storage {
        private:
            std::conditional<bHeapAllocated, std::unique_ptr<TData[]>, TData[Capacity]>::type items;

        public:
            Storage();

            TData*
            data();

            const TData*
            data() const;
        };
    } // namespace freelist_detail
} // namespace lemon

#include <lemon/shared/FreeList.inl>
