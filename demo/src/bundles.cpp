#include "bundles.h"

using namespace lemon::res;
using namespace lemon;

void
testBundleLoading()
{
    auto resMan = std::make_unique<ResourceManager>(R"(C:\git\lemon\resources)");

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
