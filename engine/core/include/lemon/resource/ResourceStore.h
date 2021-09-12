#pragma once

#include <folly/AtomicHashMap.h>
#include <folly/PackedSyncPtr.h>
#include <lemon/resource/common.h>

namespace lemon::res {
    struct ResourceContract;

    class ResourceStore {
    public:
        explicit ResourceStore(size_t size)
            :map { size } { }
        ~ResourceStore();

    private:
        folly::AtomicHashMap<uint64_t, folly::PackedSyncPtr<ResourceContract>> map;

    public:
        std::pair<ResourceContract*, bool> findOrInsert(ResourceID id);
        void remove(ResourceID id);
    };
};
