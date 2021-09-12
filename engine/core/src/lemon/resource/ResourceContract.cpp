#include <lemon/resource/ResourceContract.h>
#include <lemon/resource/ResourceInstance.h>

using namespace lemon::res;

ResourceContract::ResourceContract(ResourceLifetime lifetime)
    :contract {
    std::move(folly::makePromiseContract<void*>(lemon::scheduler::Scheduler::get()->getCPUExecutor()->weakRef()))
} { }

ResourceContract::~ResourceContract() {
    // Assume that if this destructor is called, the resource is either fully loaded, or failed to load. Either way,
    // we can destroy the data we have.
    if (contract.first.isFulfilled()) {
        auto* data = reinterpret_cast<ResourceInstance*>(contract.second.value());
        delete data;
    }
}