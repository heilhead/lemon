#include <lemon/scheduler/Scheduler.h>

using namespace lemon::scheduler;

namespace {
    void
    drawThreadEntryPoint()
    {
        auto* pScheduler = Scheduler::get();
        auto* executor =
            dynamic_cast<folly::ManualExecutor*>(pScheduler->getExecutor(TaskExecutorType::DrawThread));

        while (true) {
            executor->drain();
            executor->wait();
        }

        // TODO: Add a way to grafecully shut down the thread on scheduler destruction.
    }

    void
    renderThreadEntryPoint()
    {
        auto* pScheduler = Scheduler::get();
        auto* executor =
            dynamic_cast<folly::ManualExecutor*>(pScheduler->getExecutor(TaskExecutorType::RenderThread));

        while (true) {
            executor->drain();
            executor->wait();
        }

        // TODO: Add a way to grafecully shut down the thread on scheduler destruction.
    }
} // namespace

#if LEMON_ENABLE_THREADING
Scheduler::Scheduler(size_t numCPUThreads, size_t numIOThreads)
    : CPUPoolExecutor(numCPUThreads, 3, std::make_shared<ThreadFactory>("CPUThread")),
      IOPoolExecutor(numIOThreads, std::make_shared<ThreadFactory>("IOThread"))
{
    logger::log("starting scheduler: CPUThreadPool=", numCPUThreads, " IOThreadPool=", numIOThreads);

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
}
#else
Scheduler::Scheduler(size_t numCPUThreads, size_t numIOThreads)
{
    logger::warn("scheduler is running in single threaded mode");
}
#endif

folly::Executor*
Scheduler::getExecutor(TaskExecutorType type)
{
#if LEMON_ENABLE_THREADING
    switch (type) {
    case TaskExecutorType::DrawThread:
        return &drawThreadExecutor;
    case TaskExecutorType::RenderThread:
        return &renderThreadExecutor;
    case TaskExecutorType::CPUThreadPool:
        return &CPUPoolExecutor;
    case TaskExecutorType::IOThreadPool:
        return &IOPoolExecutor;
    case TaskExecutorType::GameThread:
    default:
        return &gameThreadExecutor;
    }
#else
    return &gameThreadExecutor;
#endif
}

void
Scheduler::processGameThreadTasks()
{
    gameThreadExecutor.drain();
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
