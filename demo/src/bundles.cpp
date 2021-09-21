#include "bundles.h"
#include <memory>
#include <lemon/resources.h>
#include <lemon/utils/utils.h>
#include <lemon/scheduler.h>
#include <folly/experimental/coro/BlockingWait.h>

using namespace lemon::res;

void
testBundleLoading() {
    std::unique_ptr<ResourceManager> resMan = std::make_unique<ResourceManager>(R"(C:\git\lemon\resources)");

    ResourceLocation location("RB_ABC");

    auto result =
        folly::coro::blockingWait(resMan->loadResource<BundleResource>(location, ResourceLifetime::Static));

    if (result) {
        lemon::utils::log("bundle successfully loaded: ", location.getFileName());
    } else {
        lemon::utils::logErr("bundle failed to load: ", location.getFileName(),
                             " error: ", (int)result.error());
    }

    auto bUnloaded = resMan->unloadResource(location);

    if (bUnloaded) {
        lemon::utils::log("bundle successfully unloaded: ", location.getFileName());
    } else {
        lemon::utils::logErr("bundle failed to unload");
    }
}

void
testModelLoading() {
    std::unique_ptr<ResourceManager> resMan = std::make_unique<ResourceManager>(R"(C:\git\lemon\resources)");

    ResourceLocation location(R"(ozz-sample\MannequinSkeleton.lem:SK_Mannequin)");

    auto result =
        folly::coro::blockingWait(resMan->loadResource<ModelResource>(location, ResourceLifetime::Static));

    if (result) {
        lemon::utils::log("model successfully loaded: ", location.getFileName());

        auto* model = (*result)->getObject<ModelResource::Model>(location);
        if (model != nullptr) {
            lemon::utils::logErr("model object: ", location.object, " name: ", model->getName());
        } else {
            lemon::utils::logErr("failed to access model object: ", location.object);
        }
    } else {
        lemon::utils::logErr("model failed to load: ", location.getFileName(),
                             " error: ", (int)result.error());
    }

    auto bUnloaded = resMan->unloadResource(location);

    if (bUnloaded) {
        lemon::utils::log("model successfully unloaded: ", location.getFileName());
    } else {
        lemon::utils::logErr("model failed to unload");
    }
}