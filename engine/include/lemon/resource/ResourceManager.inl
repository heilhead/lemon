#pragma once

#include <folly/experimental/coro/Collect.h>
#include <folly/small_vector.h>
#include <lemon/resource/ResourceInstance.h>
#include <lemon/resource/ResourceLocation.h>
#include <lemon/scheduler.h>
#include <lemon/scheduler/utils.h>
#include <lemon/utils/utils.h>

using namespace lemon::scheduler;
using namespace lemon::utils;

namespace lemon::res {
    namespace detail {
        using FactoryResultType = folly::coro::Task<ResourceContract::ResolutionType<ResourceInstance>>;

        FactoryResultType
        coResourceFactory(ResourceManager& manager, ResourceType type, const std::string& ref,
                          ResourceLifetime lifetime);

        template<typename TResource>
        Task<ResourceMetadata, ResourceLoadingError>
        coReadMetadata(const std::filesystem::path& fullPath, const std::filesystem::path& name) {
            auto result = co_await IOTask(lemon::io::coReadTextFile(fullPath));

            co_return result.map_error([](auto&& err) { return ResourceLoadingError::MetadataMissing; })
                .map([&](auto&& str) {
                    std::istringstream is(str);
                    cereal::XMLInputArchive archive(is);

                    ResourceMetadataDescriptor desc{.type = TResource::getType(),
                                                    .data = TResource::loadMetadata(archive),
                                                    .fullPath = std::move(fullPath),
                                                    .name = name};

                    return ResourceMetadata(std::move(desc));
                });
        }

        template<class TResource>
        Task<TResource*, ResourceLoadingError>
        coLoadResourceImpl(ResourceManager& manager, const ResourceLocation& location,
                           ResourceLifetime lifetime) {
            coLog("loading resource: ", location.file);
            assert(location.file != "");

            auto [pContract, bCreated] = manager.getStore().findOrInsert(location.handle);
            if (bCreated) {
                coLog("created resource: ", location.file);

                pContract->setLifetime(lifetime);

                auto metadataPath = manager.resolvePath(location);
                metadataPath += ".meta";

                auto metadataRes = co_await coReadMetadata<TResource>(metadataPath, location.file);
                if (!metadataRes) {
                    co_return tl::make_unexpected(metadataRes.error());
                }

                ResourceMetadata& metadata = *metadataRes;
                auto& refs = metadata.getReferences();
                std::vector<FactoryResultType> futRefs;

                // @TODO circular dependencies?
                for (const RawResourceReference& ref : refs) {
                    futRefs.emplace_back(
                        coResourceFactory(manager, ref.type, ref.location, ResourceLifetime::None));
                }

                std::vector<ResourceContract::ResolutionType<ResourceInstance>> resolvedDeps =
                    co_await folly::coro::collectAllRange(futRefs | ranges::views::move);
                coLog("dependencies finished: ", resolvedDeps.size());

                auto* pRes = new TResource();
                for (auto& dep : resolvedDeps) {
                    if (!dep) {
                        coLog("dependency error: ", (int)dep.error());
                        delete pRes;
                        co_return tl::make_unexpected(ResourceLoadingError::DependencyError);
                    } else {
                        pRes->addDependency(*dep);
                    }
                }

                std::optional<ResourceLoadingError> loadErr = co_await pRes->load(metadata);
                if (loadErr) {
                    coLog("resource load error: ", (int)*loadErr);
                    delete pRes;
                    co_return tl::make_unexpected(*loadErr);
                }

                // Initialization successful!
                pContract->getPromise().setValue(pRes);

                // Signal awaiting coroutines that the data is available.
                pContract->getBaton().post();
                co_return pRes;
            } else {
                coLog("found existing resource: ", location.file);

                // Wait for another thread to finish initializing the data.
                co_await pContract->getBaton();

                co_return pContract->getFuture<TResource>().value();
            }
        }
    } // namespace detail

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
    ResourceManager::loadResource(const ResourceLocation& location, ResourceLifetime lifetime) {
        static_assert(std::is_base_of_v<ResourceInstance, TResource>,
                      "TResource must be a subclass of ResourceInstance");
        return CPUTask(detail::template coLoadResourceImpl<TResource>(*this, location, lifetime));
    }
} // namespace lemon::res
