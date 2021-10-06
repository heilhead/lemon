#pragma once

#include <functional>
#include <limits>
#include <optional>
#include <lemon/shared/assert.h>
#include <folly/AtomicHashMap.h>
#include <folly/PackedSyncPtr.h>

namespace lemon {
    /// <summary>
    /// Thread-safe atomic reference counted resource handle for use with `AtomicCache`.
    /// 
    /// Notes:
    /// 
    /// - The data being pointed to by this handle is assumed to be read-only. It must
    ///   be fully initialized when inserted into the `AtomicCache`, and remain immutable
    ///   until deleted. If mutability is a requirement, a separate locking mechanism
    ///   must be implemented within the data itself.
    /// 
    /// - `ResourceRef` objects must not outlive the `AtomicCache` containing the data.
    /// </summary>
    /// <typeparam name="TData">Resource data type</typeparam>
    template<typename TData>
    class ResourceRef {
    public:
        // Only the lower 15 bits are available, so use `int16_t` max value as a limit.
        static constexpr uint16_t kRefCountLimit = std::numeric_limits<int16_t>::max();

        // Data in `AtomicCache` is never moved, so it's safe to save a pointer to it.
        // The lifetime matches `AtomicCache` lifetime.
        folly::PackedSyncPtr<TData>* ptr{nullptr};

        ResourceRef(folly::PackedSyncPtr<TData>* ptr = nullptr) noexcept : ptr{ptr}
        {
            if (ptr != nullptr) {
                acquire();
            }
        }

        ResourceRef(const ResourceRef<TData>& other) noexcept : ResourceRef(nullptr)
        {
            *this = other;
        }

        ResourceRef(ResourceRef<TData>&& other) noexcept : ResourceRef(nullptr)
        {
            *this = std::move(other);
        }

        ~ResourceRef()
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
        operator==(const ResourceRef<TData>& other) const noexcept
        {
            return ptr == other.ptr;
        }

        ResourceRef<TData>&
        operator=(const ResourceRef<TData>& other) noexcept
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

        inline ResourceRef<TData>&
        operator=(ResourceRef<TData>&& other) noexcept
        {
            if (ptr != nullptr) {
                release();
            }

            ptr = other.ptr;
            other.ptr = nullptr;

            return *this;
        }

        operator bool() const noexcept
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
        void
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

        void
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

    /// <summary>
    /// Thread-safe hashmap-based storage for cacheable objects of arbitrary types.
    /// The underlying data structure is `folly::AtomicHashMap`, which has a few
    /// important restrictions:
    /// 
    /// - `TKey` must be of an atomic integral type - either `uint32_t` or `uint64_t`.
    /// 
    /// - An estimated maximum number of elements is expected to be known at compile-time.
    ///   Attempting to insert an item into cache with no space left will result in an
    ///   exception being thrown.
    /// 
    /// Notes:
    /// 
    /// - The cache expects data to be allocated on the heap elsewhere, and only stores
    ///   a pointer to the data inside its own storage. The initialization function must
    ///   take care of allocating the data.
    /// 
    /// - The returned resource handle is `ResourceRef`, which is an atomic reference
    ///   counted pointer. Once all of the handles pointing to the data are destroyed,
    ///   the data is destroyed too.
    /// 
    /// - All of the data stored in `AtomicCache` map is destroyed along with it. For this
    ///   reason, no `ResourceRef` can outlive the `AtomicCache` as they'll be left with
    ///   a dangling pointer.
    /// </summary>
    /// <typeparam name="TData">Resource data type</typeparam>
    /// <typeparam name="TKey">An atomic integer type: either `uint32_t` or `uint64_t`</typeparam>
    template<typename TData, typename TKey = uint64_t>
    class AtomicCache {
        static constexpr size_t kSizeEst = 256;

        folly::AtomicHashMap<TKey, folly::PackedSyncPtr<TData>> data;

    public:
        using Ref = ResourceRef<TData>;

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

            // Insert a 'locked' pointer, so that we can safely initialize the data
            // in case of winning the race.
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

        inline Ref
        get(const TKey key, const std::function<TData*()>& initFn)
        {
            auto [ref, _] = findOrInsert(key, initFn);
            return std::move(ref);
        }

        inline std::optional<Ref>
        find(const TKey key)
        {
            auto search = data.find(key);
            if (search != data.end()) {
                return search->second;
            } else {
                return std::nullopt;
            }
        }
    };
} // namespace lemon
