#include "lemon/scheduler/Scheduler.h"

using namespace lemon::scheduler;

static Scheduler* gInstance;

Scheduler::Scheduler(size_t threadsIO, size_t threadsCPU) {
    assert(gInstance == nullptr);
    gInstance = this;

    poolCPU = std::make_unique<folly::CPUThreadPoolExecutor>(threadsCPU, 3);
    poolIO = std::make_unique<folly::IOThreadPoolExecutor>(threadsIO);
}

Scheduler::~Scheduler() {
    gInstance = nullptr;
}

Scheduler* Scheduler::get() {
    return gInstance;
}
