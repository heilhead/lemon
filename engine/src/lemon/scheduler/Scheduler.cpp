#include <lemon/scheduler/Scheduler.h>
#include <folly/system/ThreadName.h>

using namespace lemon::scheduler;

static Scheduler* gInstance;

Scheduler::Scheduler(size_t threadsIO, size_t threadsCPU) : poolCPU(threadsCPU, 3), poolIO(threadsIO)
{
    assert(gInstance == nullptr);
    gInstance = this;
}

Scheduler::~Scheduler()
{
    gInstance = nullptr;
}

Scheduler*
Scheduler::get()
{
    return gInstance;
}

std::optional<std::string>
Scheduler::getCurrentThreadName()
{
    auto name = folly::getCurrentThreadName();
    if (name) {
        return *name;
    } else {
        return std::nullopt;
    }
}