#include <lemon/resource/ResourceStore.h>
#include <lemon/resource/ResourceContract.h>
#include <lemon/scheduler.h>
#include <lemon/scheduler/Scheduler.h>
#include <utility>

using namespace lemon::res;
using namespace lemon::scheduler;

ResourceStore::~ResourceStore() {
    for (auto&[k, v] : map) {
        remove(k);
    }
};

std::pair<ResourceContract*, bool> ResourceStore::findOrInsert(ResourceID id) {
    folly::PackedSyncPtr<ResourceContract> pNewContract {};
    pNewContract.init();
    pNewContract.lock();

    auto[iter, bInserted] = map.insert({ id, pNewContract });
    auto& pContract = iter->second;
    if (bInserted) {
        auto contract = folly::makePromiseContract<void*>(Scheduler::get()->getCPUExecutor()->weakRef());
        pContract.set(new ResourceContract());
        pContract.unlock();
        return std::make_pair(iter->second.get(), true);
    } else {
        // Lock to make sure the contract pointer has been initialized in the above branch.
        pContract.lock();
        ResourceContract* pResult = pContract.get();
        pContract.unlock();
        return std::make_pair(pResult, false);
    }
}

void ResourceStore::remove(ResourceID id) {
    folly::PackedSyncPtr<ResourceContract>& pContract = map.find(id)->second;
    pContract.lock();

    ResourceContract* contract = pContract.get();

    if (contract != nullptr) {
        delete contract;
        pContract.set(nullptr);
    }

    pContract.unlock();
}