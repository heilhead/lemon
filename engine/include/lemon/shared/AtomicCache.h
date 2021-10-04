#pragma once

#include <functional>
#include <limits>
#include <lemon/shared/assert.h>
#include <tl/expected.hpp>
#include <folly/AtomicHashMap.h>
#include <folly/PackedSyncPtr.h>

namespace lemon {
    template<typename TData>
    class AtomicCacheRef {
    public:
        // Only the lower 15 bits are available, so use `int16_t` max value as a limit.
        static constexpr uint16_t kRefCountLimit = std::numeric_limits<int16_t>::max();

        // Data in `AtomicCache` is never moved, so it's safe to save a pointer to it.
        // The lifetime matches `AtomicCache` lifetime.
        folly::PackedSyncPtr<TData>* ptr{nullptr};

        AtomicCacheRef(folly::PackedSyncPtr<TData>* ptr = nullptr) noexcept : ptr{ptr}
        {
            if (ptr != nullptr) {
                acquire();
            }
        }

        AtomicCacheRef(const AtomicCacheRef<TData>& other) noexcept : AtomicCacheRef(nullptr)
        {
            *this = other;
        }

        AtomicCacheRef(AtomicCacheRef<TData>&& other) noexcept : AtomicCacheRef(nullptr)
        {
            *this = std::move(other);
        }

        ~AtomicCacheRef()
        {
            if (ptr != nullptr) {
                release();
            }
        }

        inline const TData&
        get() const noexcept
        {
            LEMON_ASSERT(ptr != nullptr);
            auto* pData = ptr->get();
            LEMON_ASSERT(pData != nullptr);
            return *pData;
        }

        inline const TData&
        operator*() const noexcept
        {
            return get();
        }

        inline const TData*
        operator->() const noexcept
        {
            return &get();
        }

        inline bool
        operator==(const AtomicCacheRef<TData>& other) const noexcept
        {
            return ptr == other.ptr;
        }

        AtomicCacheRef<TData>&
        operator=(const AtomicCacheRef<TData>& other) noexcept
        {
            if (ptr != nullptr) {
                release();
            }

            ptr = other.ptr;

            if (ptr != nullptr) {
                acquire();
            }

            return *this;
        }

        inline AtomicCacheRef<TData>&
        operator=(AtomicCacheRef<TData>&& other) noexcept
        {
            if (ptr != nullptr) {
                release();
            }

            ptr = other.ptr;
            other.ptr = nullptr;

            return *this;
        }

        operator bool() noexcept
        {
            if (ptr == nullptr) {
                return false;
            }

            ptr->lock();
            auto* pData = ptr->get();
            ptr->unlock();

            return pData != nullptr;
        }

        inline size_t
        refCount() const
        {
            LEMON_ASSERT(ptr != nullptr);
            return ptr->extra();
        }

    private:
        inline void
        acquire()
        {
            LEMON_ASSERT(ptr != nullptr);

            ptr->lock();
            auto refCount = ptr->extra();

            // Make sure not to overflow reference counter.
            LEMON_ASSERT(refCount < kRefCountLimit);

            ptr->setExtra(refCount + 1);
            ptr->unlock();
        }

        inline void
        release()
        {
            LEMON_ASSERT(ptr != nullptr);

            ptr->lock();
            auto refCount = ptr->extra();

            LEMON_ASSERT(refCount > 0);

            ptr->setExtra(refCount - 1);

            // The last reference, release the data.
            if (refCount == 1) {
                auto* pData = ptr->get();
                LEMON_ASSERT(pData != nullptr);
                delete pData;
                ptr->set(nullptr);
            }

            ptr->unlock();
        }
    };

    template<typename TData, typename TKey = uint64_t>
    class AtomicCache {
        static constexpr size_t kSizeEst = 256;

        folly::AtomicHashMap<TKey, folly::PackedSyncPtr<TData>> data;

    public:
        using Ref = AtomicCacheRef<TData>;

        AtomicCache(size_t sizeEst = kSizeEst) : data{kSizeEst} {}

        ~AtomicCache()
        {
            for (auto& [k, v] : data) {
                TData* pData = v.get();
                if (pData != nullptr) {
                    delete pData;
                }
            }
        }

        std::pair<Ref, bool>
        findOrInsert(const TKey key, const std::function<TData*()>& initFn)
        {
            folly::PackedSyncPtr<TData> newPtr{};
            newPtr.init();
            newPtr.lock();

            auto [search, bInserted] = data.insert({key, std::move(newPtr)});
            if (bInserted) {
                // There was no data in cache, so we need to fully initialize it.
                auto& ptr = search->second;
                ptr.set(initFn());
                ptr.unlock();
                return std::make_pair(Ref(&ptr), true);
            } else {
                // The data was found in cache. Initialize if it was destroyed.
                auto& ptr = search->second;
                ptr.lock();

                auto* result = ptr.get();
                bool bInitialized = result == nullptr;

                if (bInitialized) {
                    ptr.set(initFn());
                }

                ptr.unlock();
                return std::make_pair(Ref(&ptr), bInitialized);
            }
        }
    };
} // namespace lemon