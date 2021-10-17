#pragma once

#include <lemon/resource/common.h>

namespace lemon::res {
    struct ResourceContract;

    class ResourceStore {
        folly::AtomicHashMap<uint64_t, folly::PackedSyncPtr<ResourceContract>> map;

    public:
        explicit ResourceStore(size_t size) : map{size} {}
        ~ResourceStore();

        ResourceContract*
        find(ResourceHandle id) const;

        std::pair<ResourceContract*, bool>
        findOrInsert(ResourceHandle id);

        void
        remove(ResourceHandle id);

        void
        clear();
    };
}; // namespace lemon::res
