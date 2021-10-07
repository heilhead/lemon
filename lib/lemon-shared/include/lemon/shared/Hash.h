#pragma once

#include <vector>
#include <folly/Hash.h>

namespace lemon {
    using StringID = uint64_t;

    struct Hash {
    private:
        uint64_t hash;

    public:
        template<typename... Value>
        inline void
        append(Value const&... value)
        {
            appendHash(folly::hash::commutative_hash_combine(value...));
        }

        template<typename T>
        inline void
        append(std::vector<T> const& data)
        {
            appendRange(data.begin(), data.end());
        }

        template<class Iter>
        inline void
        appendRange(Iter first, Iter last)
        {
            appendHash(folly::hash::hash_range<Iter, folly::Hash>(first, last));
        }

        template<class Iter>
        inline void
        appendRangeCommutative(Iter first, Iter last)
        {
            appendHash(folly::hash::commutative_hash_combine_range(first, last));
        }

        inline uint64_t
        value() const
        {
            return hash;
        }

        inline operator uint64_t() const
        {
            return value();
        }

        inline void
        appendHash(uint64_t inHash)
        {
            hash = folly::hash::hash_128_to_64(hash, inHash);
        }
    };

    constexpr StringID
    sid(const char* str)
    {
        return folly::hash::fnva64_buf(str, std::char_traits<char>::length(str));
    }

    inline StringID
    sid(const std::string& str)
    {
        return sid(str.c_str());
    }

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
