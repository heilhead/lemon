#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>
#include <cassert>

template<size_t N>
class Arena {
    static const size_t alignment = 4;

    alignas(alignment) uint8_t buffer[N] {};
    uint8_t* ptr = nullptr;

public:
    Arena()
        :ptr { buffer } { }
//    Arena(const Arena&) = delete;
//    Arena& operator=(const Arena&) = delete;

    ~Arena() {
        ptr = nullptr;
    }

    uint8_t* allocate(size_t n) {
        assert(ptr != nullptr);
        n = align(n);
        if (buffer + N - ptr >= n) {
            auto p = ptr;
            ptr += n;
            return p;
        } else {
            return static_cast<uint8_t*>(::operator new(n));
        }
    }

    void deallocate(uint8_t* p, size_t n) {
        assert(ptr != nullptr);
        if (contains(p)) {
            n = align(n);
            if (p + n == ptr) {
                ptr = p;
            }
        } else {
            ::operator delete(p);
        }
    }

private:
    inline bool contains(const uint8_t* p) {
        return buffer <= p && p <= (buffer + N);
    }

    inline size_t align(size_t n) {
        return (n + (alignment - 1)) & ~(alignment - 1);
    }
};


