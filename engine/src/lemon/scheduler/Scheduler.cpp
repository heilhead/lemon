#include <lemon/scheduler/Scheduler.h>

using namespace lemon::scheduler;

namespace {
    void
    drawThreadEntryPoint()
    {
        auto* pScheduler = Scheduler::get();
        auto* executor = pScheduler->getDrawThreadExecutor();
        auto* CPUPool = pScheduler->getCPUExecutor();

        while (true) {
            executor->drain();
            executor->wait();
        }
    }

    void
    renderThreadEntryPoint()
    {
        auto* pScheduler = Scheduler::get();
        auto* executor = pScheduler->getRenderThreadExecutor();

        while (true) {
            executor->drain();
            executor->wait();
        }
    }
} // namespace

Scheduler::Scheduler(size_t threadsIO, size_t threadsCPU)
    : poolCPU(threadsCPU, 3, std::make_shared<ThreadFactory>("CPUThread")),
      poolIO(threadsIO, std::make_shared<ThreadFactory>("IOThread"))
{
#if LEMON_FORCE_SINGLE_THREADED
    logger::warn("scheduler is running in single threaded mode");
#else
    logger::log("starting scheduler: CPUThreadPool=", threadsCPU, " IOThreadPool=", threadsIO);

    drawThread = std::thread([]() {
        constexpr auto name = "DrawThread";
        OPTICK_THREAD(name);
        folly::setThreadName(name);
        drawThreadEntryPoint();
    });

    renderThread = std::thread([]() {
        constexpr auto name = "RenderThread";
        OPTICK_THREAD(name);
        folly::setThreadName(name);
        renderThreadEntryPoint();
    });
#endif
}

void
Scheduler::processGameThreadTasks()
{
    getGameThreadExecutor()->drain();
}

std::optional<std::string>
Scheduler::getThreadName()
{
    auto name = folly::getCurrentThreadName();
    if (name) {
        return *name;
    } else {
        return std::nullopt;
    }
}
