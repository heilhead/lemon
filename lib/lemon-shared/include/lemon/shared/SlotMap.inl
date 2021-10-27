#pragma once

lemon::SlotMapHandle::SlotMapHandle(uint32_t index, uint32_t generation)
    : index{index}, generation{generation}
{
}

inline uint32_t
lemon::SlotMapHandle::getIndex() const
{
    return index;
}

inline uint32_t
lemon::SlotMapHandle::getGeneration() const
{
    return generation;
}

inline bool
lemon::SlotMapHandle::operator==(const SlotMapHandle& other) const
{
    return index == other.index && generation == other.generation;
}

template<typename TData, uint32_t Capacity>
lemon::SlotMap<TData, Capacity>::SlotMap() : keys{}, data{}, keyLookup{}, size{0}
{
    for (size_t i = 0; i < Capacity - 1; i++) {
        keys[i].setNext(&keys[i + 1]);
    }

    keys[Capacity - 1].setNext(nullptr);
    pNextKey = &keys[0];
}

template<typename TData, uint32_t Capacity>
lemon::SlotMap<TData, Capacity>::~SlotMap()
{
    clear();
}

template<typename TData, uint32_t Capacity>
template<typename... TArgs>
[[nodiscard]] lemon::SlotMapHandle
lemon::SlotMap<TData, Capacity>::insert(TArgs&&... args)
{
    LEMON_ASSERT(size < Capacity);

    auto dataIndex = size++;
    auto keyIndex = popKey();

    auto& key = keys[keyIndex];
    key.dataIndex = dataIndex;

    keyLookup[dataIndex] = keyIndex;
    new (assumeInit<TData>(&data[dataIndex])) TData(std::forward<TArgs>(args)...);

    return SlotMapHandle(keyIndex, key.generation);
}

template<typename TData, uint32_t Capacity>
bool
lemon::SlotMap<TData, Capacity>::remove(SlotMapHandle handle)
{
    if (isValid(handle)) {
        removeImpl(keys[handle.getIndex()].dataIndex);

        return true;
    }

    return false;
}

template<typename TData, uint32_t Capacity>
bool
lemon::SlotMap<TData, Capacity>::remove(size_t index)
{
    if (index < size) {
        removeImpl(index);

        return true;
    }

    return false;
}

template<typename TData, uint32_t Capacity>
lemon::SlotMapHandle
lemon::SlotMap<TData, Capacity>::getHandle(size_t index) const
{
    LEMON_ASSERT(index < size);

    auto keyIndex = keyLookup[index];
    auto& key = keys[keyIndex];

    return SlotMapHandle(keyIndex, key.generation);
}

template<typename TData, uint32_t Capacity>
void
lemon::SlotMap<TData, Capacity>::clear()
{
    while (size > 0) {
        removeImpl(size - 1);
    }
}

template<typename TData, uint32_t Capacity>
bool
lemon::SlotMap<TData, Capacity>::isValid(SlotMapHandle handle)
{
    auto keyIndex = handle.getIndex();

    LEMON_ASSERT(keyIndex < Capacity);

    auto& key = keys[keyIndex];

    return key.isUsed() && key.generation == handle.getGeneration();
}

template<typename TData, uint32_t Capacity>
size_t
lemon::SlotMap<TData, Capacity>::getSize() const
{
    return size;
}

template<typename TData, uint32_t Capacity>
size_t
lemon::SlotMap<TData, Capacity>::getCapacity() const
{
    return Capacity;
}

template<typename TData, uint32_t Capacity>
const TData&
lemon::SlotMap<TData, Capacity>::operator[](SlotMapHandle handle) const
{
    LEMON_ASSERT(isValid(handle));

    auto keyIndex = handle.getIndex();
    auto dataIndex = keys[keyIndex].dataIndex;

    return getData(dataIndex);
}

template<typename TData, uint32_t Capacity>
TData&
lemon::SlotMap<TData, Capacity>::operator[](SlotMapHandle handle)
{
    LEMON_ASSERT(isValid(handle));

    auto keyIndex = handle.getIndex();
    auto dataIndex = keys[keyIndex].dataIndex;

    return getData(dataIndex);
}

template<typename TData, uint32_t Capacity>
const TData&
lemon::SlotMap<TData, Capacity>::operator[](size_t index) const
{
    return getData(index);
}

template<typename TData, uint32_t Capacity>
TData&
lemon::SlotMap<TData, Capacity>::operator[](size_t index)
{
    return getData(index);
}

template<typename TData, uint32_t Capacity>
inline const TData&
lemon::SlotMap<TData, Capacity>::getData(size_t index) const
{
    LEMON_ASSERT(index < size);
    return *assumeInit<TData>(&data[index]);
}

template<typename TData, uint32_t Capacity>
inline TData&
lemon::SlotMap<TData, Capacity>::getData(size_t index)
{
    LEMON_ASSERT(index < size);
    return *assumeInit<TData>(&data[index]);
}

template<typename TData, uint32_t Capacity>
void
lemon::SlotMap<TData, Capacity>::removeImpl(size_t currDataIndex)
{
    auto lastDataIndex = --size;

    auto* pCurrData = assumeInit<TData>(&data[currDataIndex]);
    pCurrData->~TData();

    pushKey(keyLookup[currDataIndex]);

    // Perform swap.
    if (currDataIndex != lastDataIndex) {
        auto* pLastData = assumeInit<TData>(&data[lastDataIndex]);

        new (pCurrData) TData(std::move(*pLastData));
        keyLookup[currDataIndex] = keyLookup[lastDataIndex];

        auto& key = keys[keyLookup[currDataIndex]];
        key.dataIndex = currDataIndex;
    }
}

template<typename TData, uint32_t Capacity>
size_t
lemon::SlotMap<TData, Capacity>::popKey()
{
    LEMON_ASSERT(pNextKey != nullptr);

    auto keyIndex = getKeyIndex(pNextKey);
    auto& key = keys[keyIndex];

    LEMON_ASSERT(!key.isUsed());

    key.setUsed(true);
    pNextKey = key.getNext();

    return keyIndex;
}

template<typename TData, uint32_t Capacity>
void
lemon::SlotMap<TData, Capacity>::pushKey(size_t keyIndex)
{
    auto& key = keys[keyIndex];

    LEMON_ASSERT(key.isUsed());

    key.setNext(pNextKey);
    key.setUsed(false);
    key.bump();

    pNextKey = &key;
}

template<typename TData, uint32_t Capacity>
inline size_t
lemon::SlotMap<TData, Capacity>::getKeyIndex(const Key* ptr) const
{
    LEMON_ASSERT(ptr != nullptr);
    ptrdiff_t idx = ptr - &keys[0];
    LEMON_ASSERT(idx >= 0 && idx < Capacity);
    return static_cast<size_t>(idx);
}

lemon::slotmap_detail::SlotMapKey::SlotMapKey() : dataIndex{0}, generation{0}, next{}
{
    next.init(nullptr, 0);
}

inline void
lemon::slotmap_detail::SlotMapKey::setNext(SlotMapKey* pNext)
{
    next.set(pNext);
}

inline lemon::slotmap_detail::SlotMapKey*
lemon::slotmap_detail::SlotMapKey::getNext() const
{
    return next.get();
}

inline bool
lemon::slotmap_detail::SlotMapKey::isUsed() const
{
    return next.extra() == 1;
}

inline void
lemon::slotmap_detail::SlotMapKey::setUsed(bool bUsed)
{
    return next.setExtra(bUsed ? 1 : 0);
}

inline void
lemon::slotmap_detail::SlotMapKey::bump()
{
    generation++;
}
