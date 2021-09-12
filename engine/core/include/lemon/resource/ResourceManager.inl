#pragma once

#include <lemon/scheduler.h>
#include <lemon/resource/ResourceInstance.h>
#include <lemon/resource/ResourceLocation.h>
#include <folly/small_vector.h>
#include <lemon/utils/utils.h>
#include <folly/experimental/coro/Collect.h>

using namespace lemon::scheduler;
using namespace lemon::utils;

namespace lemon::res {
    namespace detail {
        using FactoryResultType = folly::coro::Task<ResourceContract::ResolutionType<ResourceInstance>>;

        FactoryResultType
        resourceFactory(ResourceManager& manager, ResourceType type, ResourceLocation location);

        template<class TResource>
        Task<TResource*, ResourceLoadingError> loadImpl(ResourceManager& manager, ResourceLocation& location) {
            tprint("loading resource: ", location.file);
            assert(location.file != "");

            // @TODO add 'file exists' validation?
            auto[pContract, bCreated] = manager.getStore().findOrInsert(location.handle);
            if (bCreated) {
                tprint("created resource: ", location.file);

                // 1. load leaf dependencies @TODO circular dependencies?
                // 3. resolve dependencies for `ResourceInstance`

                // @TODO missing metadata doesn't generate error?
                auto metadataRes = manager.loadMetadata<TResource>(location);
                if (!metadataRes) {
                    co_return tl::make_unexpected(metadataRes.error());
                }

                ResourceMetadata& metadata = *metadataRes;
                auto& refs = metadata.getReferences();
                std::vector<FactoryResultType> futRefs;

                for (const RawResourceReference& ref: refs) {
                    futRefs.emplace_back(resourceFactory(manager, ref.type, ResourceLocation(ref.location)));
                }

                std::vector<ResourceContract::ResolutionType<ResourceInstance>> resolvedDeps =
                    co_await folly::coro::collectAllRange(futRefs | ranges::views::move);
                tprint("dependencies finished: ", resolvedDeps.size());

                for (auto& dep : resolvedDeps) {
                    if (dep) {
                        tprint("dependency error: ", (int)dep.error());
                    } else {
                        tprint("dependency loaded: ", (uintptr_t)*dep);
                    }
                }

                auto* pRes = new TResource();
                if (!pRes->init(metadata)) {
                    delete pRes;
                    co_return tl::make_unexpected(ResourceLoadingError::InitializationError);
                }

                // Initialization successful!
                pContract->getPromise().setValue(pRes);

                // Signal awaiting coroutines that the data is available.
                pContract->getBaton().post();
                co_return pRes;
            } else {
                tprint("found existing resource: ", location.file);

                // Wait for another thread to finish initializing the data.
                co_await pContract->getBaton();
                auto& future = pContract->getFuture<TResource>();
                co_return future.value();
            }
        }
    }

    template<typename TResource>
    tl::expected<ResourceMetadata, ResourceLoadingError>
    ResourceManager::loadMetadata(ResourceLocation& location) {
        auto fullPath = resolvePath(location);
        fullPath += ".meta";

        return lemon::io::readTextFile(fullPath)
            .map_error([](auto&& err) {
                return ResourceLoadingError::MetadataMissing;
            })
            .map([](auto&& str) {
                std::istringstream is(str);
                cereal::XMLInputArchive archive(is);
                return ResourceMetadata(TResource::getType(), TResource::loadMetadata(archive));
            });
    }

    template<typename TResource>
    TResource*
    ResourceManager::getResource(ResourceHandle handle) {
        ResourceContract* pContract = getContract(handle);
        if (pContract == nullptr) {
            return nullptr;
        }

        auto& promise = pContract->template getPromise<TResource>();
        if (!promise.isFulfilled()) {
            return nullptr;
        }

        auto& future = pContract->template getFuture<TResource>();
        auto& resolution = future.value();
        if (!resolution) {
            return nullptr;
        }

        return *resolution;
    }

    template<class TResource>
    ResourceContract::FutureType<TResource>
    ResourceManager::loadResource(ResourceLocation& location, ResourceLifetime lifetime) {
        static_assert(std::is_base_of_v<ResourceInstance, TResource>,
            "TResource must be a subclass of ResourceInstance");
        return CPUTask(detail::template loadImpl<TResource>(*this, location));
    }
}
