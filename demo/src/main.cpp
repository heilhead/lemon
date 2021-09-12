#include <iostream>
#include <mutex>

#include <lemon/scheduler.h>
#include <folly/experimental/coro/BlockingWait.h>
#include <folly/experimental/coro/Collect.h>

#include <lemon/shared.h>
#include <lemon/shared/filesystem.h>
#include <lemon/resource/ResourceManager.h>

#include <cereal/archives/xml.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/unordered_map.hpp>
#include <cereal/types/memory.hpp>

#include <folly/PackedSyncPtr.h>
#include <lemon/resource/ResourceLocation.h>
#include <lemon/resource/ResourceContract.h>
#include <lemon/resource/types/MaterialResource.h>

std::mutex mut;

template<typename ...Args>
void print(Args&& ...args) {
    std::lock_guard lg(mut);
    (std::cout << ... << args) << std::endl;
}

using namespace lemon::scheduler;
using namespace lemon::res;

// This can accept e.g. a callback that returns `MaterialInstance*`
Task<int, int> contractTest() {
    auto[pContract, bCreated] = lemon::res::ResourceManager::get()->getStore().findOrInsert(1);
    if (bCreated) {
        print("1: new contract");

        // 0. await metadata
        // 1. load leaf dependencies @TODO circular dependencies?
        // 2. call callback that returns the resource ptr e.g. `ResourceInstance*`.
        // 3. resolve dependencies for `ResourceInstance`
        // 4. initialize `ResourceInstance` (e.g. load own data)
        // 5. expose subobjects somehow @TODO should subobjects be `ResourceInstance` or something else?

        auto* pRes = new MaterialResource();

        pContract->getPromise()->setValue(pRes);

        // load metadata
        // [optional] init resource object (e.g. add dependencies)
        // await dependencies
        // load own data
        // [optional] expose subobjects
        // resolve promise
        // return data
    } else {
        print("2: existing contract");
        // await future
        // return data
    }

    co_return 0;
}

int main(int argc, char* argv[]) {
    std::unique_ptr<Scheduler> schedMan = std::make_unique<Scheduler>(24, 24);
    std::unique_ptr<lemon::res::ResourceManager> resMan = std::make_unique<lemon::res::ResourceManager>(
        R"(C:\git\lemon\resources)");

    std::vector<TaskFuture<int, int>> tasks;
    for (int i = 0; i < 32; i++) {
        tasks.emplace_back(CPUTask(contractTest()));
    }

    auto result = folly::coro::blockingWait(folly::collectAll(tasks.begin(), tasks.end()));

    resMan->getStore().remove(1);

    std::string path = "basketball.mat";
    auto meta = resMan->loadMetadata<MaterialResource>(path);

    if (meta) {
        auto* matData = (*meta).get<MaterialResource::Metadata>();
        print("load metadata: success");
    } else {
        print("load metadata: error");
    }

    return 0;
}
