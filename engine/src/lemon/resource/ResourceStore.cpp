#include <lemon/resource/ResourceContract.h>
#include <lemon/resource/ResourceStore.h>
#include <utility>
#include <lemon/shared/logger.h>

using namespace lemon::res;
using namespace lemon::scheduler;

ResourceStore::~ResourceStore()
{
    for (auto& [k, v] : map) {
        remove(k);
    }
};

ResourceContract*
ResourceStore::find(ResourceHandle id)
{
    auto it = map.find(id);
    if (it == map.end()) {
        return nullptr;
    } else {
        folly::PackedSyncPtr<ResourceContract>& ptr = it->second;
        ptr.lock();
        ResourceContract* pContract = ptr.get();
        ptr.unlock();
        return pContract;
    }
}

std::pair<ResourceContract*, bool>
ResourceStore::findOrInsert(ResourceHandle handle)
{
    folly::PackedSyncPtr<ResourceContract> pNewContract{};
    pNewContract.init();
    pNewContract.lock();

    auto [iter, bInserted] = map.insert({handle, pNewContract});
    auto& pContract = iter->second;
    if (bInserted) {
        auto contract = folly::makePromiseContract<void*>(Scheduler::get()->getCPUExecutor()->weakRef());
        pContract.set(new ResourceContract());
        pContract.unlock();
        return {iter->second.get(), true};
    } else {
        // Lock to make sure the contract pointer has been initialized in the above branch.
        pContract.lock();
        ResourceContract* pResult = pContract.get();
        pContract.unlock();
        return {pResult, false};
    }
}

void
ResourceStore::remove(ResourceHandle handle)
{
    auto it = map.find(handle);
    if (it == map.end()) {
        logger::err("failed to remove resource: handle ", handle.get(), " not found");
        return;
    }

    folly::PackedSyncPtr<ResourceContract>& pContract = it->second;
    pContract.lock();

    ResourceContract* contract = pContract.get();

    if (contract != nullptr) {
        delete contract;
        pContract.set(nullptr);
    }

    pContract.unlock();
}
