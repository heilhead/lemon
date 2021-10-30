#pragma once

namespace lemon {
    template<SlotMapDataType TData, uint32_t Capacity, SlotMapHandleType THandle>
    SlotMap<TData, Capacity, THandle>::SlotMap() : keys{}, data{}, keyLookup{}, size{0}
    {
        for (size_t i = 0; i < Capacity - 1; i++) {
            keys[i].setNext(&keys[i + 1]);
        }

        keys[Capacity - 1].setNext(nullptr);
        pNextKey = &keys[0];
    }

    template<SlotMapDataType TData, uint32_t Capacity, SlotMapHandleType THandle>
    SlotMap<TData, Capacity, THandle>::~SlotMap()
    {
        clear();
    }

    template<SlotMapDataType TData, uint32_t Capacity, SlotMapHandleType THandle>
    template<typename... TArgs>
    [[nodiscard]] THandle
    SlotMap<TData, Capacity, THandle>::insert(TArgs&&... args)
    {
        LEMON_ASSERT(size < Capacity);

        auto dataIndex = size++;
        auto keyIndex = popKey();

        auto& key = keys[keyIndex];
        key.dataIndex = dataIndex;

        keyLookup[dataIndex] = keyIndex;
        data[dataIndex].init(std::forward<TArgs>(args)...);

        return THandle(keyIndex, key.generation);
    }

    template<SlotMapDataType TData, uint32_t Capacity, SlotMapHandleType THandle>
    bool
    SlotMap<TData, Capacity, THandle>::remove(THandle handle)
    {
        if (isValid(handle)) {
            removeImpl(keys[handle.getIndex()].dataIndex);

            return true;
        }

        return false;
    }

    template<SlotMapDataType TData, uint32_t Capacity, SlotMapHandleType THandle>
    bool
    SlotMap<TData, Capacity, THandle>::remove(size_t index)
    {
        if (index < size) {
            removeImpl(index);

            return true;
        }

        return false;
    }

    template<SlotMapDataType TData, uint32_t Capacity, SlotMapHandleType THandle>
    THandle
    SlotMap<TData, Capacity, THandle>::getHandle(size_t index) const
    {
        LEMON_ASSERT(index < size);

        auto keyIndex = keyLookup[index];
        auto& key = keys[keyIndex];

        return THandle(keyIndex, key.generation);
    }

    template<SlotMapDataType TData, uint32_t Capacity, SlotMapHandleType THandle>
    void
    SlotMap<TData, Capacity, THandle>::clear()
    {
        while (size > 0) {
            removeImpl(size - 1);
        }
    }

    template<SlotMapDataType TData, uint32_t Capacity, SlotMapHandleType THandle>
    bool
    SlotMap<TData, Capacity, THandle>::isValid(THandle handle)
    {
        auto keyIndex = handle.getIndex();
        if (keyIndex == kInvalidIndex) {
            return false;
        }

        LEMON_ASSERT(keyIndex < Capacity);

        auto& key = keys[keyIndex];

        return key.isUsed() && key.generation == handle.getGeneration();
    }

    template<SlotMapDataType TData, uint32_t Capacity, SlotMapHandleType THandle>
    size_t
    SlotMap<TData, Capacity, THandle>::getSize() const
    {
        return size;
    }

    template<SlotMapDataType TData, uint32_t Capacity, SlotMapHandleType THandle>
    size_t
    SlotMap<TData, Capacity, THandle>::getCapacity() const
    {
        return Capacity;
    }

    template<SlotMapDataType TData, uint32_t Capacity, SlotMapHandleType THandle>
    const TData&
    SlotMap<TData, Capacity, THandle>::operator[](THandle handle) const
    {
        LEMON_ASSERT(isValid(handle));

        auto keyIndex = handle.getIndex();
        auto dataIndex = keys[keyIndex].dataIndex;

        return getData(dataIndex);
    }

    template<SlotMapDataType TData, uint32_t Capacity, SlotMapHandleType THandle>
    TData&
    SlotMap<TData, Capacity, THandle>::operator[](THandle handle)
    {
        LEMON_ASSERT(isValid(handle));

        auto keyIndex = handle.getIndex();
        auto dataIndex = keys[keyIndex].dataIndex;

        return getData(dataIndex);
    }

    template<SlotMapDataType TData, uint32_t Capacity, SlotMapHandleType THandle>
    const TData&
    SlotMap<TData, Capacity, THandle>::operator[](size_t index) const
    {
        return getData(index);
    }

    template<SlotMapDataType TData, uint32_t Capacity, SlotMapHandleType THandle>
    TData&
    SlotMap<TData, Capacity, THandle>::operator[](size_t index)
    {
        return getData(index);
    }

    template<SlotMapDataType TData, uint32_t Capacity, SlotMapHandleType THandle>
    inline const TData&
    SlotMap<TData, Capacity, THandle>::getData(size_t index) const
    {
        LEMON_ASSERT(index < size);
        return *data[index];
    }

    template<SlotMapDataType TData, uint32_t Capacity, SlotMapHandleType THandle>
    inline TData&
    SlotMap<TData, Capacity, THandle>::getData(size_t index)
    {
        LEMON_ASSERT(index < size);
        return *data[index];
    }

    template<SlotMapDataType TData, uint32_t Capacity, SlotMapHandleType THandle>
    const TData*
    SlotMap<TData, Capacity, THandle>::getData(THandle handle) const
    {
        auto keyIndex = handle.getIndex();
        if (keyIndex == kInvalidIndex) {
            return nullptr;
        }

        LEMON_ASSERT(keyIndex < Capacity);

        auto& key = keys[keyIndex];
        if (key.isUsed() && key.generation == handle.getGeneration()) {
            return &getData(key.dataIndex);
        } else {
            return nullptr;
        }
    }

    template<SlotMapDataType TData, uint32_t Capacity, SlotMapHandleType THandle>
    TData*
    SlotMap<TData, Capacity, THandle>::getData(THandle handle)
    {
        auto keyIndex = handle.getIndex();
        if (keyIndex == kInvalidIndex) {
            return nullptr;
        }

        LEMON_ASSERT(keyIndex < Capacity);

        auto& key = keys[keyIndex];
        if (key.isUsed() && key.generation == handle.getGeneration()) {
            return &getData(key.dataIndex);
        } else {
            return nullptr;
        }
    }

    template<SlotMapDataType TData, uint32_t Capacity, SlotMapHandleType THandle>
    void
    SlotMap<TData, Capacity, THandle>::removeImpl(size_t currDataIndex)
    {
        auto lastDataIndex = --size;

        data[currDataIndex].destroy();

        pushKey(keyLookup[currDataIndex]);

        // Perform swap.
        if (currDataIndex != lastDataIndex) {
            data[currDataIndex].init(std::move(*data[lastDataIndex]));
            data[lastDataIndex].destroy();

            keyLookup[currDataIndex] = keyLookup[lastDataIndex];

            auto& key = keys[keyLookup[currDataIndex]];
            key.dataIndex = currDataIndex;
        }
    }

    template<SlotMapDataType TData, uint32_t Capacity, SlotMapHandleType THandle>
    size_t
    SlotMap<TData, Capacity, THandle>::popKey()
    {
        LEMON_ASSERT(pNextKey != nullptr);

        auto keyIndex = getKeyIndex(pNextKey);
        auto& key = keys[keyIndex];

        LEMON_ASSERT(!key.isUsed());

        key.setUsed(true);
        pNextKey = key.getNext();

        return keyIndex;
    }

    template<SlotMapDataType TData, uint32_t Capacity, SlotMapHandleType THandle>
    void
    SlotMap<TData, Capacity, THandle>::pushKey(size_t keyIndex)
    {
        auto& key = keys[keyIndex];

        LEMON_ASSERT(key.isUsed());

        key.setNext(pNextKey);
        key.setUsed(false);
        key.bump();

        pNextKey = &key;
    }

    template<SlotMapDataType TData, uint32_t Capacity, SlotMapHandleType THandle>
    inline size_t
    SlotMap<TData, Capacity, THandle>::getKeyIndex(const Key* ptr) const
    {
        LEMON_ASSERT(ptr != nullptr);
        ptrdiff_t idx = ptr - &keys[0];
        LEMON_ASSERT(idx >= 0 && idx < Capacity);
        return static_cast<size_t>(idx);
    }
} // namespace lemon
