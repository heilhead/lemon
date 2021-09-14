#pragma once

#include <folly/experimental/coro/Collect.h>
#include <folly/small_vector.h>
#include <lemon/resource/ResourceInstance.h>
#include <lemon/resource/ResourceLocation.h>
#include <lemon/scheduler.h>
#include <lemon/utils/utils.h>

using namespace lemon::scheduler;
using namespace lemon::utils;

namespace lemon::res {
    namespace detail {
        template<typename TResource>
        tl::expected<ResourceMetadata, ResourceLoadingError>
        parseMetadata(tl::expected<std::string, lemon::io::Error>&& data,
                      const std::filesystem::path&& fullPath, const std::filesystem::path& name) {
            return data.map_error([](auto&& err) { return ResourceLoadingError::MetadataMissing; })
                .map([&](auto&& str) {
                    std::istringstream is(str);
                    cereal::XMLInputArchive archive(is);

                    ResourceMetadataDescriptor desc{.type = ResourceManager::getClassID<TResource>(),
                                                    .data = TResource::loadMetadata(archive),
                                                    .fullPath = fullPath,
                                                    .name = name};

                    return ResourceMetadata(std::move(desc));
                });
        }

        FactoryResultType
        coResourceFactory(ResourceClassID refType, const std::string& refLocation, ResourceLifetime lifetime);

        template<typename TResource>
        Task<ResourceMetadata, ResourceLoadingError>
        coReadMetadata(const std::filesystem::path&& fullPath, const std::filesystem::path& name) {
            auto result = co_await IOTask(lemon::io::coReadTextFile(fullPath));
            co_return parseMetadata<TResource>(std::move(result), std::move(fullPath), name);
        }

        template<class TResource>
        Task<TResource*, ResourceLoadingError>
        coLoadResourceImpl(const ResourceLocation& location, ResourceLifetime lifetime) {
            auto* manager = ResourceManager::get();

            lemon::utils::print("loading resource: ", location.file);
            assert(location.file != "");

            auto [pContract, bCreated] = manager->getStore().findOrInsert(location.handle);
            if (bCreated) {
                lemon::utils::print("created resource: ", location.file);

                pContract->setLifetime(lifetime);

                auto metadataPath = manager->resolvePath(location);
                metadataPath += ".meta";

                auto metadataRes = co_await coReadMetadata<TResource>(std::move(metadataPath), location.file);
                if (!metadataRes) {
                    co_return tl::make_unexpected(metadataRes.error());
                }

                ResourceMetadata& metadata = *metadataRes;
                auto& refs = metadata.getReferences();
                std::vector<FactoryResultType> futRefs;

                // @TODO circular dependencies?
                for (const RawResourceReference& ref : refs) {
                    futRefs.emplace_back(coResourceFactory(ref.type, ref.location, ResourceLifetime::None));
                }

                std::vector<ResourceContract::ResolutionType<ResourceInstance>> resolvedDeps =
                    co_await folly::coro::collectAllRange(futRefs | ranges::views::move);
                lemon::utils::print("dependencies finished: ", resolvedDeps.size());

                auto* pRes = new TResource();
                pRes->setHandle(location.handle);

                for (auto& dep : resolvedDeps) {
                    if (!dep) {
                        lemon::utils::print("dependency error: ", (int)dep.error());
                        delete pRes;
                        co_return tl::make_unexpected(ResourceLoadingError::DependencyError);
                    } else {
                        pRes->addDependency(*dep);
                    }
                }

                std::optional<ResourceLoadingError> loadErr = co_await pRes->load(metadata);
                if (loadErr) {
                    lemon::utils::print("resource load error: ", (int)*loadErr);
                    delete pRes;
                    co_return tl::make_unexpected(*loadErr);
                }

                // Initialization successful!
                pContract->getPromise().setValue(pRes);

                // Signal awaiting coroutines that the data is available.
                pContract->getBaton().post();
                co_return pRes;
            } else {
                lemon::utils::print("found existing resource: ", location.file);

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
        return CPUTask(detail::template coLoadResourceImpl<TResource>(location, lifetime));
    }

    template<typename TResource>
    tl::expected<ResourceMetadata, ResourceLoadingError>
    ResourceManager::loadMetadata(const ResourceLocation& location) {
        auto fullPath = resolvePath(location);
        fullPath += ".meta";
        return detail::parseMetadata<TResource>(lemon::io::readTextFile(fullPath), std::move(fullPath),
                                                location.file);
    }

    template<typename TResource>
    void
    ResourceManager::registerClass() {
        auto type = getClassID<TResource>();
        auto factory = [](const std::string& ref, ResourceLifetime lifetime) -> FactoryResultType {
            ResourceLocation location(ref);
            lemon::utils::print("resourceFactory: classID=", ResourceManager::getClassID<TResource>(),
                                " location.file=", location.file);
            co_return(co_await lemon::res::detail::coLoadResourceImpl<TResource>(location, lifetime))
                .map([](TResource* v) { return reinterpret_cast<ResourceInstance*>(v); });
        };

        factories.insert({type, factory});
    }
} // namespace lemon::res
