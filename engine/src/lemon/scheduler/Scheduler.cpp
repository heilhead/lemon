#include <lemon/scheduler/Scheduler.h>
#include <lemon/shared/assert.h>
#include <folly/system/ThreadName.h>

using namespace lemon::scheduler;

static Scheduler* gInstance;

Scheduler::Scheduler(size_t threadsIO, size_t threadsCPU) : poolCPU(threadsCPU, 3), poolIO(threadsIO)
{
    LEMON_ASSERT(gInstance == nullptr);
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