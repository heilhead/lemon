#include <iostream>
#include <mutex>

#include <folly/experimental/coro/BlockingWait.h>
#include <folly/experimental/coro/Collect.h>
#include <lemon/scheduler.h>

#include <lemon/resource/ResourceManager.h>
#include <lemon/shared.h>
#include <lemon/shared/filesystem.h>

#include <cereal/archives/xml.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/types/unordered_map.hpp>
#include <cereal/types/vector.hpp>

#include <folly/PackedSyncPtr.h>
#include <lemon/resource/ResourceContract.h>
#include <lemon/resource/ResourceLocation.h>
#include <lemon/resource/types/MaterialResource.h>

std::mutex mut;

template<typename... Args>
void
print(Args&&... args) {
    std::lock_guard lg(mut);
    (std::cout << ... << args) << std::endl;
}

using namespace lemon::scheduler;
using namespace lemon::res;

void
run() {
    std::unique_ptr<lemon::res::ResourceManager> resMan =
        std::make_unique<lemon::res::ResourceManager>(R"(C:\git\lemon\resources)");

    std::string matPath = "M_Basketball";
    ResourceLocation location(matPath);

    tprint("resource state 1: ", (int)location.handle.getState());

    auto result = folly::coro::blockingWait(resMan->loadResource<MaterialResource>(location));
    auto* ptr = *result;

    tprint("resource state 2: ", (int)location.handle.getState());
    auto* matPtr = resMan->getResource<MaterialResource>(location.handle);
    assert(ptr == matPtr);

    tprint("mat ptr: ", (uintptr_t)matPtr);

    resMan->getStore().remove(location.handle);

    tprint("resource state 3: ", (int)location.handle.getState());

    // auto meta = resMan->loadMetadata<MaterialResource>(location);
    // if (meta) {
    //     auto* matData = (*meta).get<MaterialResource::Metadata>();
    //     tprint("load metadata: success");
    // } else {
    //     tprint("load metadata: error");
    // }
}

int
main(int argc, char* argv[]) {
    std::unique_ptr<Scheduler> schedMan = std::make_unique<Scheduler>(24, 24);

    run();
    // run();
    // run();
    // run();

    return 0;
}
