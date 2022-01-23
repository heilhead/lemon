#include <lemon/resource/ResourceContract.h>
#include <lemon/resource/ResourceInstance.h>

using namespace lemon::res;
using namespace lemon::scheduler;

ResourceContract::ResourceContract()
{
    folly::Executor* executor;

#if LEMON_FORCE_SINGLE_THREADED
    executor = Scheduler::get()->getDebugExecutor();
#else
    executor = Scheduler::get()->getCPUExecutor();
#endif

    auto executorToken = folly::Executor::getKeepAliveToken(executor);
    auto [tmpPromise, tmpFuture] = folly::makePromiseContract<ResolutionType<void>>(executorToken);
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
