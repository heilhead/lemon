#pragma once

namespace lemon {
    template<SlotMapDataType TData, uint32_t Capacity>
    SlotMap<TData, Capacity>::SlotMap() : keys{}, data{}, keyLookup{}, size{0}
    {
        for (size_t i = 0; i < Capacity - 1; i++) {
            keys[i].setNext(&keys[i + 1]);
        }

        keys[Capacity - 1].setNext(nullptr);
        pNextKey = &keys[0];
    }

    template<SlotMapDataType TData, uint32_t Capacity>
    SlotMap<TData, Capacity>::~SlotMap()
    {
        clear();
    }

    template<SlotMapDataType TData, uint32_t Capacity>
    template<typename... TArgs>
    [[nodiscard]] SlotMapHandle
    SlotMap<TData, Capacity>::insert(TArgs&&... args)
    {
        LEMON_ASSERT(size < Capacity);

        auto dataIndex = size++;
        auto keyIndex = popKey();

        auto& key = keys[keyIndex];
        key.dataIndex = dataIndex;

        keyLookup[dataIndex] = keyIndex;
        data[dataIndex].init(std::forward<TArgs>(args)...);

        return SlotMapHandle(keyIndex, key.generation);
    }

    template<SlotMapDataType TData, uint32_t Capacity>
    bool
    SlotMap<TData, Capacity>::remove(SlotMapHandle handle)
    {
        if (isValid(handle)) {
            removeImpl(keys[handle.getIndex()].dataIndex);

            return true;
        }

        return false;
    }

    template<SlotMapDataType TData, uint32_t Capacity>
    bool
    SlotMap<TData, Capacity>::remove(size_t index)
    {
        if (index < size) {
            removeImpl(index);

            return true;
        }

        return false;
    }

    template<SlotMapDataType TData, uint32_t Capacity>
    SlotMapHandle
    SlotMap<TData, Capacity>::getHandle(size_t index) const
    {
        LEMON_ASSERT(index < size);

        auto keyIndex = keyLookup[index];
        auto& key = keys[keyIndex];

        return SlotMapHandle(keyIndex, key.generation);
    }

    template<SlotMapDataType TData, uint32_t Capacity>
    void
    SlotMap<TData, Capacity>::clear()
    {
        while (size > 0) {
            removeImpl(size - 1);
        }
    }

    template<SlotMapDataType TData, uint32_t Capacity>
    bool
    SlotMap<TData, Capacity>::isValid(SlotMapHandle handle)
    {
        auto keyIndex = handle.getIndex();
        if (keyIndex == kInvalidIndex) {
            return false;
        }

        LEMON_ASSERT(keyIndex < Capacity);

        auto& key = keys[keyIndex];

        return key.isUsed() && key.generation == handle.getGeneration();
    }

    template<SlotMapDataType TData, uint32_t Capacity>
    size_t
    SlotMap<TData, Capacity>::getSize() const
    {
        return size;
    }

    template<SlotMapDataType TData, uint32_t Capacity>
    size_t
    SlotMap<TData, Capacity>::getCapacity() const
    {
        return Capacity;
    }

    template<SlotMapDataType TData, uint32_t Capacity>
    const TData&
    SlotMap<TData, Capacity>::operator[](SlotMapHandle handle) const
    {
        LEMON_ASSERT(isValid(handle));

        auto keyIndex = handle.getIndex();
        auto dataIndex = keys[keyIndex].dataIndex;

        return getData(dataIndex);
    }

    template<SlotMapDataType TData, uint32_t Capacity>
    TData&
    SlotMap<TData, Capacity>::operator[](SlotMapHandle handle)
    {
        LEMON_ASSERT(isValid(handle));

        auto keyIndex = handle.getIndex();
        auto dataIndex = keys[keyIndex].dataIndex;

        return getData(dataIndex);
    }

    template<SlotMapDataType TData, uint32_t Capacity>
    const TData&
    SlotMap<TData, Capacity>::operator[](size_t index) const
    {
        return getData(index);
    }

    template<SlotMapDataType TData, uint32_t Capacity>
    TData&
    SlotMap<TData, Capacity>::operator[](size_t index)
    {
        return getData(index);
    }

    template<SlotMapDataType TData, uint32_t Capacity>
    inline const TData&
    SlotMap<TData, Capacity>::getData(size_t index) const
    {
        LEMON_ASSERT(index < size);
        return *data[index];
    }

    template<SlotMapDataType TData, uint32_t Capacity>
    inline TData&
    SlotMap<TData, Capacity>::getData(size_t index)
    {
        LEMON_ASSERT(index < size);
        return *data[index];
    }

    template<SlotMapDataType TData, uint32_t Capacity>
    void
    SlotMap<TData, Capacity>::removeImpl(size_t currDataIndex)
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

    template<SlotMapDataType TData, uint32_t Capacity>
    size_t
    SlotMap<TData, Capacity>::popKey()
    {
        LEMON_ASSERT(pNextKey != nullptr);

        auto keyIndex = getKeyIndex(pNextKey);
        auto& key = keys[keyIndex];

        LEMON_ASSERT(!key.isUsed());

        key.setUsed(true);
        pNextKey = key.getNext();

        return keyIndex;
    }

    template<SlotMapDataType TData, uint32_t Capacity>
    void
    SlotMap<TData, Capacity>::pushKey(size_t keyIndex)
    {
        auto& key = keys[keyIndex];

        LEMON_ASSERT(key.isUsed());

        key.setNext(pNextKey);
        key.setUsed(false);
        key.bump();

        pNextKey = &key;
    }

    template<SlotMapDataType TData, uint32_t Capacity>
    inline size_t
    SlotMap<TData, Capacity>::getKeyIndex(const Key* ptr) const
    {
        LEMON_ASSERT(ptr != nullptr);
        ptrdiff_t idx = ptr - &keys[0];
        LEMON_ASSERT(idx >= 0 && idx < Capacity);
        return static_cast<size_t>(idx);
    }
} // namespace lemon
