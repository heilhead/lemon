#pragma once

#include <folly/Hash.h>

namespace lemon {
    struct Hash {
    private:
        uint64_t hash;

    public:
        template<typename... Value>
        void
        append(Value const&... value)
        {
            appendHash(folly::hash::commutative_hash_combine(value...));
        }

        template<class Iter>
        void
        appendRange(Iter first, Iter last)
        {
            appendHash(folly::hash::hash_range(first, last));
        }

        template<class Iter>
        void
        appendRangeCommutative(Iter first, Iter last)
        {
            appendHash(folly::hash::commutative_hash_combine_range(first, last));
        }

        inline uint64_t
        value() const
        {
            return hash;
        }

        operator uint64_t() const
        {
            return value();
        }

        void
        appendHash(uint64_t inHash)
        {
            hash = folly::hash::hash_128_to_64(hash, inHash);
        }
    };

    template<typename... Value>
    uint64_t
    hash(Value const&... value)
    {
        Hash h;
        h.append(value...);
        return h;
    }

    template<class Iter>
    uint64_t
    hashRange(Iter first, Iter last)
    {
        Hash h;
        h.appendRange(first, last);
        return h;
    }

    template<class Iter>
    uint64_t
    hashRangeCommutative(Iter first, Iter last)
    {
        Hash h;
        h.appendRangeCommutative(first, last);
        return h;
    }
} // namespace lemon