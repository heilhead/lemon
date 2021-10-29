#pragma once
namespace lemon {
    template<typename TData, size_t Capacity, bool bHeapAllocated>
    FreeList<TData, Capacity, bHeapAllocated>::FreeList()
    {
        for (size_t i = 0; i < Capacity - 1; i++) {
            getItem(i)->pNext = getItem(i + 1);
        }

        getItem(Capacity - 1)->pNext = nullptr;
        pHead = getItem(0);
        freeCount = Capacity;
    }

    template<typename TData, size_t Capacity, bool bHeapAllocated>
    FreeList<TData, Capacity, bHeapAllocated>::~FreeList() noexcept
    {
        LEMON_ASSERT(freeCount == Capacity);
    }

    template<typename TData, size_t Capacity, bool bHeapAllocated>
    template<typename... TArgs>
    size_t
    FreeList<TData, Capacity, bHeapAllocated>::insert(TArgs&&... args)
    {
        // TODO: Figure out a better API, maybe return an `std::optional<size_t>`?
        LEMON_ASSERT(freeCount > 0);
        LEMON_ASSERT(pHead != nullptr);

        freeCount--;
        auto* pOldHead = pHead;
        pHead = pOldHead->pNext;

        const auto* pItem = pOldHead->data.init(std::forward<TArgs>(args)...);

        return getIndex(pItem);
    }

    template<typename TData, size_t Capacity, bool bHeapAllocated>
    inline void
    FreeList<TData, Capacity, bHeapAllocated>::remove(size_t index)
    {
        remove(getData(index));
    }

    template<typename TData, size_t Capacity, bool bHeapAllocated>
    void
    FreeList<TData, Capacity, bHeapAllocated>::remove(TData* pData)
    {
        auto idx = getIndex(pData);

        auto* pItem = getItem(idx);
        pItem->data.destroy();

        auto* pOldHead = pHead;
        pHead = pItem;
        pHead->pNext = pOldHead;

        freeCount++;
    }

    template<typename TData, size_t Capacity, bool bHeapAllocated>
    inline size_t
    FreeList<TData, Capacity, bHeapAllocated>::getFreeCount()
    {
        return freeCount;
    }

    template<typename TData, size_t Capacity, bool bHeapAllocated>
    inline size_t
    FreeList<TData, Capacity, bHeapAllocated>::getCapacity()
    {
        return Capacity;
    }

    template<typename TData, size_t Capacity, bool bHeapAllocated>
    inline size_t
    FreeList<TData, Capacity, bHeapAllocated>::getIndex(const TData* ptr) const
    {
        LEMON_ASSERT(ptr != nullptr);
        ptrdiff_t idx = reinterpret_cast<const DataWrapper*>(ptr) - getItem(0);
        LEMON_ASSERT(idx >= 0 && idx < Capacity);
        return static_cast<size_t>(idx);
    }

    template<typename TData, size_t Capacity, bool bHeapAllocated>
    inline TData&
    FreeList<TData, Capacity, bHeapAllocated>::operator[](size_t index)
    {
        return *getData(index);
    }

    template<typename TData, size_t Capacity, bool bHeapAllocated>
    inline const TData&
    FreeList<TData, Capacity, bHeapAllocated>::operator[](size_t index) const
    {
        return *getData(index);
    }

    template<typename TData, size_t Capacity, bool bHeapAllocated>
    inline FreeList<TData, Capacity, bHeapAllocated>::DataWrapper*
    FreeList<TData, Capacity, bHeapAllocated>::getItem(size_t index)
    {
        LEMON_ASSERT(index < Capacity);
        return &storage.data()[index];
    }

    template<typename TData, size_t Capacity, bool bHeapAllocated>
    inline const FreeList<TData, Capacity, bHeapAllocated>::DataWrapper*
    FreeList<TData, Capacity, bHeapAllocated>::getItem(size_t index) const
    {
        LEMON_ASSERT(index < Capacity);
        return &storage.data()[index];
    }

    template<typename TData, size_t Capacity, bool bHeapAllocated>
    inline const TData*
    FreeList<TData, Capacity, bHeapAllocated>::getData(size_t index) const
    {
        LEMON_ASSERT(index < Capacity);
        return getItem(index)->data.assumeInit();
    }

    template<typename TData, size_t Capacity, bool bHeapAllocated>
    inline TData*
    FreeList<TData, Capacity, bHeapAllocated>::getData(size_t index)
    {
        LEMON_ASSERT(index < Capacity);
        return getItem(index)->data.assumeInit();
    }

    namespace freelist_detail {
        template<typename TData, size_t Capacity, bool bHeapAllocated>
        Storage<TData, Capacity, bHeapAllocated>::Storage()
        {
            if constexpr (bHeapAllocated) {
                items = std::unique_ptr<TData[]>(new TData[Capacity]);
            }
        }

        template<typename TData, size_t Capacity, bool bHeapAllocated>
        inline TData*
        Storage<TData, Capacity, bHeapAllocated>::data()
        {
            if constexpr (bHeapAllocated) {
                return items.get();
            } else {
                return &items[0];
            }
        }

        template<typename TData, size_t Capacity, bool bHeapAllocated>
        inline const TData*
        Storage<TData, Capacity, bHeapAllocated>::data() const
        {
            if constexpr (bHeapAllocated) {
                return items.get();
            } else {
                return &items[0];
            }
        }
    } // namespace freelist_detail
} // namespace lemon
