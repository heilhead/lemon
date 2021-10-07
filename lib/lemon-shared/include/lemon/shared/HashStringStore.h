#pragma once

#include <folly/AtomicHashMap.h>
#include <folly/PackedSyncPtr.h>

namespace lemon {
    // TODO: Replace its uses with `AtomicCache` and remove.
    class HashStringStore {
    public:
        explicit HashStringStore(size_t sizeEst = kDefaultPoolEstimate);
        ~HashStringStore();

    private:
        static constexpr size_t kDefaultPoolEstimate = 512;
        folly::AtomicHashMap<uint64_t, folly::PackedSyncPtr<const std::string>> map;

    public:
        inline uint64_t
        hash(const std::string& str)
        {
            return folly::hash::fnva64(str);
        }

        void
        insert(uint64_t hash, const std::string& str);

        inline uint64_t
        insert(const std::string& str)
        {
            auto hh = hash(str);
            insert(hh, str);
            return hh;
        }

        const std::string*
        find(uint64_t hash);
    };
} // namespace lemon