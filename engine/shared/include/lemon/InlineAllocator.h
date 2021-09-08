#pragma once

#include <cstddef>
#include <iostream>
#include "lemon/Arena.h"

template<typename T, size_t N>
struct AsBytes {
    static constexpr size_t byteLength = sizeof(T) * N;
    alignas(alignof(T)) uint8_t data[byteLength];

    template<typename U>
    AsBytes(const AsBytes<U, N>& other) { }
};

template<typename T, size_t N>
class InlineAllocator {
public:
    typedef T value_type;
    typedef T* pointer;
    typedef T* const_pointer;
    typedef T* void_pointer;
    typedef T* const_void_pointer;

    template<class U>
    struct rebind {
        typedef InlineAllocator<U, N> other;
    };

    InlineAllocator() noexcept = default;

    template<typename U>
    explicit InlineAllocator(const InlineAllocator<U, N>& other) {
//        inlineItems = std::move(other.inlineItems);
    };

    InlineAllocator& operator=(const InlineAllocator&) = delete;

    T* allocate(size_t n) {
//        return reinterpret_cast<T*>(inlineItems.allocate(n * sizeof(T)));
        return nullptr;
    }

    void deallocate(T* ptr, size_t n) {
//        inlineItems.deallocate(reinterpret_cast<uint8_t*>(ptr), n * sizeof(T));
    }

//private:
//    AsBytes<T, N> inlineItems;
};

template<class T, class U, size_t N, size_t M>
constexpr bool operator==(const InlineAllocator<T, N>& a, const InlineAllocator<U, M>& b) noexcept {
    return N == M && &a.inlineItems == &b.inlineItems;
}

template<class T, class U, size_t N, size_t M>
constexpr bool operator!=(const InlineAllocator<T, N>& a, const InlineAllocator<U, M>& b) noexcept {
    return !(a == b);
}
