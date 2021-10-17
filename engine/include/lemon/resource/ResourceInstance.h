#pragma once

#include <lemon/scheduler.h>
#include <lemon/resource/common.h>

using namespace lemon::scheduler;

namespace lemon::res {
    struct ResourceMetadata;
    struct ResourceLocation;

    class ResourceObject {
        friend class ResourceInstance;

    public:
        virtual ~ResourceObject() = default;

    private:
        ResourceObjectHandle handle = ResourceObjectHandle::InvalidHandle;

    public:
        inline ResourceObjectHandle
        getHandle() const
        {
            return handle;
        }

    private:
        inline void
        setHandle(ResourceObjectHandle inHandle)
        {
            handle = inHandle;
        }
    };

    /**
     * @TODO
     *  - have generation counter
     *  - add pruning based on generations to `ResourceStore`
     */
    class ResourceInstance : private NonMovable {
        friend class ResourceManager;

    public:
        static constexpr size_t kObjectCapacity = 8;
        static constexpr size_t kDependencyCapacity = 8;

        ResourceInstance() {}

        virtual ~ResourceInstance();

    private:
        folly::small_vector<ResourceObject*> objects;
        folly::small_vector<ResourceInstance*, kDependencyCapacity> dependencies;
        ResourceHandle handle{ResourceHandle::InvalidHandle};
        std::atomic<uint32_t> dependants{0};

    public:
        template<class TObject = ResourceObject>
        const TObject*
        getObject(ResourceObjectHandle handle) const;

        template<class TObject = ResourceObject>
        const TObject*
        getObject(const ResourceLocation& location) const;

        inline void
        setHandle(ResourceHandle inHandle)
        {
            handle = inHandle;
        }

        [[nodiscard]] inline ResourceHandle
        getHandle() const
        {
            return handle;
        }

        virtual VoidTask<ResourceLoadingError>
        load(ResourceMetadata&& meta) = 0;

        void
        addDependency(ResourceInstance* pResource);

    protected:
        template<class TObject = ResourceObject, typename... Args>
        void
        createObject(ResourceObjectHandle handle, Args&&... args);

    private:
        inline uint32_t
        getDependantCount() const
        {
            return dependants.load(std::memory_order_relaxed);
        }

        inline void addDependant(/*ResourceInstance* pResource*/)
        {
            dependants++;
        }

        inline void removeDependant(/*ResourceInstance* pResource*/)
        {
            dependants--;
        }

        void
        detachDependencies(std::vector<ResourceHandle>* unusedDeps);
    };
} // namespace lemon::res

#include <lemon/resource/ResourceInstance.inl>
