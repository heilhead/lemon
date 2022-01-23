#pragma once

#include <vector>
#include <string>
#include <gsl/gsl>
#include <folly/Hash.h>

namespace lemon {
    using StringID = uint64_t;

    struct Hash {
    private:
        uint64_t hash{0};

    public:
        Hash() = default;

        template<typename... Value>
        Hash(const Value&... value) : Hash()
        {
            append(value...);
        }

        template<typename... Value>
        inline void
        append(const Value&... value)
        {
            appendHash(folly::hash::commutative_hash_combine(value...));
        }

        template<typename T>
        inline void
        append(const std::vector<T>& data)
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
    sid(gsl::czstring<> str)
    {
        return folly::hash::fnva64_buf(str, std::char_traits<char>::length(str));
    }

    constexpr StringID
    sid(gsl::czstring<> str, size_t sz)
    {
        return folly::hash::fnva64_buf(str, sz);
    }

    inline StringID
    sid(const std::string& str)
    {
        return sid(str.c_str());
    }

    inline StringID
    sid(const std::string_view& view)
    {
        return sid(view.data(), view.size());
    }

    template<typename... Value>
    uint64_t
    hash(const Value&... value)
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
