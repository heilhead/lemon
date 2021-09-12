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

int main(int argc, char* argv[]) {
    std::unique_ptr<Scheduler> schedMan = std::make_unique<Scheduler>(24, 24);
    std::unique_ptr<lemon::res::ResourceManager> resMan = std::make_unique<lemon::res::ResourceManager>(
        R"(C:\git\lemon\resources)");

    std::string matPath = "M_Basketball";
    ResourceLocation location(matPath);

    tprint("resource state 1: ", (int)location.handle.getState());

//    std::vector<TaskFuture<int, int>> tasks;
//    for (int i = 0; i < 32; i++) {
//        tasks.emplace_back(CPUTask(contractTest()));
//    }

//    auto result = folly::coro::blockingWait(folly::collectAll(tasks.begin(), tasks.end()));

    auto result = folly::coro::blockingWait(resMan->loadResource<MaterialResource>(location));
    auto* ptr = *result;

    tprint("resource state 2: ", (int)location.handle.getState());
    auto* matPtr = resMan->getResource<MaterialResource>(location.handle);
    assert(ptr == matPtr);

    tprint("mat ptr: ", (uintptr_t)matPtr);

    resMan->getStore().remove(location.handle);

    tprint("resource state 3: ", (int)location.handle.getState());

    auto meta = resMan->loadMetadata<MaterialResource>(location);

    if (meta) {
        auto* matData = (*meta).get<MaterialResource::Metadata>();
        tprint("load metadata: success");
    } else {
        tprint("load metadata: error");
    }

    return 0;
}
