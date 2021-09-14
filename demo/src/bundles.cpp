#include "bundles.h"
#include <memory>
#include <lemon/resources.h>
#include <lemon/utils/utils.h>
#include <lemon/scheduler.h>
#include <folly/experimental/coro/BlockingWait.h>

using namespace lemon::res;

/*

@TODO:
- print resource manager stats (i.e. live object count etc.)
- finalize resource manager api
    - decide whether to use location or handles
    - decide if there's need in `ResourceLocation` and subobject handles
- tests?

*/

void
testBundleLoading() {
    std::unique_ptr<ResourceManager> resMan = std::make_unique<ResourceManager>(R"(C:\git\lemon\resources)");

    ResourceLocation location("RB_ABC");

    auto result =
        folly::coro::blockingWait(resMan->loadResource<BundleResource>(location, ResourceLifetime::Static));

    if (result) {
        lemon::utils::print("bundle successfully loaded: ", location.file);
    } else {
        lemon::utils::print("bundle failed to load: ", location.file, " error: ", (int)result.error());
    }

    auto bUnloaded = resMan->unloadResource(location);

    if (bUnloaded) {
        lemon::utils::print("bundle successfully unloaded: ", location.file);
    } else {
        lemon::utils::print("bundle failed to unload");
    }
}