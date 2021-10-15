#include <lemon/resource/ResourceContract.h>
#include <lemon/resource/ResourceInstance.h>

using namespace lemon::res;
using namespace lemon::scheduler;

ResourceContract::ResourceContract()
{
    auto exec = Scheduler::get()->getCPUExecutor()->weakRef();
    auto [tmpPromise, tmpFuture] = folly::makePromiseContract<ResolutionType<void>>(exec);
    promise = std::move(tmpPromise);
    future = std::move(tmpFuture);
}

ResourceContract::~ResourceContract()
{
    // Assume that if this destructor is called, the resource is either fully loaded, or failed to load.
    // Either way, we can destroy the data we have.
    if (promise.isFulfilled()) {
        auto& resolution = future.value();
        if (resolution) {
            // The resource is fully initialized, so we should destroy it.
            auto* data = reinterpret_cast<ResourceInstance*>(resolution.value());
            delete data;
        }
    }
}
