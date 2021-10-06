#pragma once

#include <folly/experimental/coro/Collect.h>
#include <folly/small_vector.h>
#include <lemon/resource/ResourceInstance.h>
#include <lemon/resource/ResourceLocation.h>
#include <lemon/scheduler.h>
#include <lemon/utils/utils.h>
#include <lemon/shared/assert.h>

using namespace lemon::scheduler;
using namespace lemon::utils;

namespace lemon::res {
    namespace res_detail {
        template<class TResource>
        tl::expected<ResourceMetadata, ResourceLoadingError>
        parseMetadata(tl::expected<std::string, lemon::io::Error>&& data,
                      const std::filesystem::path&& fullPath, const std::string& name)
        {
            return data.map_error([](auto&& err) { return ResourceLoadingError::MetadataMissing; })
                .map([&](auto&& str) {
                    std::istringstream is(str);
                    cereal::YAMLInputArchive archive(is);

                    ResourceMetadataDescriptor desc{.type = ResourceManager::getClassID<TResource>(),
                                                    .data = TResource::loadMetadata(archive),
                                                    .fullPath = fullPath,
                                                    .name = name};

                    return ResourceMetadata(std::move(desc));
                });
        }

        FactoryResultType
        coResourceFactory(ResourceClassID classID, const std::string& refLocation, ResourceLifetime lifetime);

        template<class TResource>
        Task<ResourceMetadata, ResourceLoadingError>
        coReadMetadata(const std::filesystem::path&& fullPath, const std::string& name)
        {
            auto result = co_await IOTask(lemon::io::coReadTextFile(fullPath));
            co_return parseMetadata<TResource>(std::move(result), std::move(fullPath), name);
        }

        template<class TResource>
        Task<TResource*, ResourceLoadingError>
        coLoadResourceImpl(const ResourceLocation& location, ResourceLifetime lifetime)
        {
            auto* manager = ResourceManager::get();

            auto& fileName = location.getFileName();
            LEMON_ASSERT(fileName != "");

            lemon::utils::log("loading resource: ", fileName);

            auto [pContract, bCreated] = manager->getStore().findOrInsert(location.handle);
            if (bCreated) {
                lemon::utils::log("created resource: ", fileName);

                pContract->setLifetime(lifetime);

                auto metadataPath = manager->resolvePath(location);
                metadataPath += ".meta";

                auto metadataRes = co_await coReadMetadata<TResource>(std::move(metadataPath), fileName);
                if (!metadataRes) {
                    co_return tl::make_unexpected(metadataRes.error());
                }

                auto metadata = std::move(metadataRes.value());
                auto& refs = metadata.getReferences();
                std::vector<FactoryResultType> futRefs;

                // @TODO circular dependencies?
                for (const RawResourceReference& ref : refs) {
                    futRefs.emplace_back(coResourceFactory(ref.type, ref.location, ResourceLifetime::None));
                }

                std::vector<ResourceContract::ResolutionType<ResourceInstance>> resolvedDeps =
                    co_await folly::coro::collectAllRange(futRefs | ranges::views::move);
                lemon::utils::log("dependencies finished: ", resolvedDeps.size());

                // Use `unique_ptr` here so that it's easier to destroy the data if bailing on error.
                auto pRes = std::make_unique<TResource>();
                pRes->setHandle(location.handle);

                for (auto& dep : resolvedDeps) {
                    if (!dep) {
                        lemon::utils::logErr("dependency error: ", (int)dep.error());
                        co_return tl::make_unexpected(ResourceLoadingError::DependencyError);
                    } else {
                        pRes->addDependency(*dep);
                    }
                }

                std::optional<ResourceLoadingError> loadErr = co_await pRes->load(std::move(metadata));
                if (loadErr) {
                    lemon::utils::logErr("resource load error: ", (int)*loadErr);
                    co_return tl::make_unexpected(*loadErr);
                }

                if (location.object.isValid()) {
                    auto* pSubObject = pRes->getObject(location.object);
                    if (pSubObject == nullptr) {
                        lemon::utils::logErr("resource load error: subobject not available");
                        co_return tl::make_unexpected(ResourceLoadingError::ObjectMissing);
                    }
                }

                // Initialization successful! Release the pointer and settle the contract.
                auto* pRawRes = pRes.release();

                pContract->getPromise().setValue(pRawRes);

                // Signal the awaiting coroutines that the data is now available.
                pContract->getBaton().post();

                co_return pRawRes;
            } else {
                lemon::utils::log("found existing resource: ", fileName);

                // Wait for another thread to finish initializing the data.
                co_await pContract->getBaton();

                co_return pContract->getFuture<TResource>().value();
            }
        }
    } // namespace res_detail

    template<class TResource>
    TResource*
    ResourceManager::getResource(ResourceHandle handle)
    {
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
    ResourceManager::loadResource(const ResourceLocation& location, ResourceLifetime lifetime)
    {
        static_assert(std::is_base_of_v<ResourceInstance, TResource>,
                      "TResource must be a subclass of ResourceInstance");
        return CPUTask(res_detail::coLoadResourceImpl<TResource>(location, lifetime));
    }

    template<class TResource>
    tl::expected<ResourceMetadata, ResourceLoadingError>
    ResourceManager::loadMetadata(const ResourceLocation& location)
    {
        auto fullPath = resolvePath(location);
        fullPath += ".meta";

        return res_detail::parseMetadata<TResource>(lemon::io::readTextFile(fullPath), std::move(fullPath),
                                                    location.getFileName());
    }

    template<class TResource>
    void
    ResourceManager::registerClass()
    {
        auto classID = getClassID<TResource>();
        auto factory = [](const std::string& ref, ResourceLifetime lifetime) -> FactoryResultType {
            // The reference string received here may include subobject, e.g. `some\resource:SubObjectName`,
            // so construct a `ResourceLocation` object from it to identify both the resource and the
            // subobject.
            ResourceLocation location(ref);

            lemon::utils::log("resourceFactory: classID=", ResourceManager::getClassID<TResource>(),
                              " location.file=", location.getFileName());

            co_return(co_await res_detail::coLoadResourceImpl<TResource>(location, lifetime))
                .map([](TResource* v) { return reinterpret_cast<ResourceInstance*>(v); });
        };

        LEMON_ASSERT(factories.find(classID) == factories.end());

        factories.insert({classID, factory});
    }
} // namespace lemon::res
