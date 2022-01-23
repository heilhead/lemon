#include <lemon/scheduler/Scheduler.h>

using namespace lemon::scheduler;

Scheduler::Scheduler(size_t threadsIO, size_t threadsCPU) : poolCPU(threadsCPU, 3), poolIO(threadsIO)
{
#if LEMON_FORCE_SINGLE_THREADED
    logger::warn("scheduler is running in single threaded mode");
#else
    logger::log("starting scheduler: CPUThreadPool=", threadsCPU, " IOThreadPool=", threadsIO);
#endif
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
