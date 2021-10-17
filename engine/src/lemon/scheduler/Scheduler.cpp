#include <lemon/scheduler/Scheduler.h>

using namespace lemon::scheduler;

Scheduler::Scheduler(size_t threadsIO, size_t threadsCPU) : poolCPU(threadsCPU, 3), poolIO(threadsIO) {}

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
