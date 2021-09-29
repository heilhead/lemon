#pragma once

#include <functional>
#include <tl/expected.hpp>
#include <folly/AtomicHashMap.h>
#include <folly/PackedSyncPtr.h>

namespace lemon {
    template<typename TKey, typename TData>
    class AtomicCache {
        static constexpr size_t kSizeEst = 256;

        folly::AtomicHashMap<TKey, folly::PackedSyncPtr<TData>> data;

    public:
        AtomicCache(size_t sizeEst = kSizeEst) : data{kSizeEst} {}

        ~AtomicCache()
        {
            for (auto& [k, v] : data) {
                delete v.get();
            }
        }

        std::pair<const TData*, bool>
        findOrInsert(const TKey key, const std::function<TData*()>& initFn)
        {
            folly::PackedSyncPtr<TData> ptr{};
            ptr.init();
            ptr.lock();

            auto [search, bInserted] = data.insert({key, std::move(ptr)});
            if (bInserted) {
                auto& ptr = search->second;
                auto* result = initFn();
                ptr.set(result);
                ptr.unlock();
                return {result, true};
            } else {
                ptr.lock();
                auto* result = ptr.get();
                ptr.unlock();
                return {result, false};
            }
        }
    };
} // namespace lemon