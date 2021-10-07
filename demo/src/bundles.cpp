#include "bundles.h"
#include <memory>
#include <lemon/resources.h>
#include <lemon/utils/utils.h>
#include <lemon/scheduler.h>
#include <folly/experimental/coro/BlockingWait.h>

using namespace lemon::res;
using namespace lemon;

void
testBundleLoading()
{
    std::unique_ptr<ResourceManager> resMan = std::make_unique<ResourceManager>(R"(C:\git\lemon\resources)");

    ResourceLocation location("RB_ABC");

    auto result =
        folly::coro::blockingWait(resMan->loadResource<BundleResource>(location, ResourceLifetime::Static));

    if (result) {
        logger::log("bundle successfully loaded: ", location.getFileName());
    } else {
        logger::err("bundle failed to load: ", location.getFileName(), " error: ", (int)result.error());
    }

    auto bUnloaded = resMan->unloadResource(location);

    if (bUnloaded) {
        logger::log("bundle successfully unloaded: ", location.getFileName());
    } else {
        logger::err("bundle failed to unload");
    }
}

void
testModelLoading()
{
    std::unique_ptr<ResourceManager> resMan = std::make_unique<ResourceManager>(R"(C:\git\lemon\resources)");

    ResourceLocation location(R"(ozz-sample\MannequinSkeleton.lem:SK_Mannequin)");

    auto result =
        folly::coro::blockingWait(resMan->loadResource<ModelResource>(location, ResourceLifetime::Static));

    if (result) {
        logger::log("model successfully loaded: ", location.getFileName());

        auto* model = (*result)->getObject<ModelResource::Model>(location);
        if (model != nullptr) {
            logger::err("model object: ", location.object, " name: ", model->getName());
        } else {
            logger::err("failed to access model object: ", location.object);
        }
    } else {
        logger::err("model failed to load: ", location.getFileName(), " error: ", (int)result.error());
    }

    auto bUnloaded = resMan->unloadResource(location);

    if (bUnloaded) {
        logger::log("model successfully unloaded: ", location.getFileName());
    } else {
        logger::err("model failed to unload");
    }
}
