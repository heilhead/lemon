#pragma once

#include <lemon/resource/common.h>
#include <folly/small_vector.h>
#include <atomic>
#include <lemon/utils/utils.h>

namespace lemon::res {
    struct ResourceMetadata;

    class ResourceObject {
        friend class ResourceInstance;

    public:
        virtual ~ResourceObject() = default;

    private:
        ResourceObjectHandle handle = ResourceObjectHandle::InvalidHandle;

    public:
        inline ResourceObjectHandle getHandle() const {
            return handle;
        }

    private:
        inline void setHandle(ResourceObjectHandle inHandle) {
            handle = inHandle;
        }
    };

    /**
     * @TODO
     *  - have generation counter
     *  - add pruning based on generations to `ResourceStore`
     */
    class ResourceInstance {
        friend class ResourceManager;

    public:
        static constexpr size_t kObjectCapacity = 8;
        static constexpr size_t kDependencyCapacity = 8;

        ResourceInstance() {
            lemon::utils::tprint("ResourceInstance::ResourceInstance()");
        }

        virtual ~ResourceInstance();

    private:
        folly::small_vector<ResourceObject*, kObjectCapacity> objects;
        folly::small_vector<ResourceInstance*, kDependencyCapacity> dependencies;

    protected:
        ResourceHandle handle { ResourceHandle::InvalidHandle };

    private:
        std::atomic<uint32_t> dependants { 0 };

    public:
        template<class T = ResourceObject>
        const T* getObject(ResourceObjectHandle handle) const;

        inline void setHandle(ResourceHandle inHandle) {
            handle = inHandle;
        }

        [[nodiscard]]
        inline ResourceHandle getHandle() const {
            return handle;
        }

        virtual bool init(ResourceMetadata& meta) = 0;

    protected:
        template<class T = ResourceObject, typename ...Args>
        void createObject(ResourceObjectHandle handle, Args&& ... args);

    private:
        void addDependency(ResourceInstance* pResource);

        inline uint32_t getDependantCount() const {
            return dependants.load(std::memory_order_relaxed);
        }

        inline void addDependant(/*ResourceInstance* pResource*/) {
            dependants++;
        }

        inline void removeDependant(/*ResourceInstance* pResource*/) {
            dependants--;
        }
    };
}

#include <lemon/resource/ResourceInstance.inl>
