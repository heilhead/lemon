#pragma once

#include <lemon/scheduler/common.h>
#include <lemon/profiling.h>

namespace lemon::scheduler {
    class ThreadFactory : public folly::ThreadFactory {
        std::string prefix;
        std::atomic<uint64_t> suffix;

    public:
        explicit ThreadFactory(folly::StringPiece inPrefix) : prefix(inPrefix.str()), suffix(0) {}

        std::thread
        newThread(folly::Func&& func) override
        {
            auto name = folly::to<std::string>(prefix, suffix++);
            return std::thread([func = std::move(func), name = std::move(name)]() mutable {
                OPTICK_THREAD(name.c_str());
                folly::setThreadName(name);
                func();
            });
        }

        void
        setNamePrefix(folly::StringPiece inPrefix)
        {
            prefix = inPrefix.str();
        }

        const std::string&
        getNamePrefix()
        {
            return prefix;
        }
    };

    class Scheduler : public UnsafeSingleton<Scheduler> {
    public:
        Scheduler(size_t threadsIO = std::thread::hardware_concurrency(),
                  size_t threadsCPU = std::thread::hardware_concurrency());

    private:
        folly::IOThreadPoolExecutor poolIO;
        folly::CPUThreadPoolExecutor poolCPU;
        folly::ManualExecutor gameThreadExecutor;
        folly::ManualExecutor drawThreadExecutor;
        folly::ManualExecutor renderThreadExecutor;
        std::thread drawThread;
        std::thread renderThread;

#if LEMON_FORCE_SINGLE_THREADED
        folly::ManualExecutor debugExecutor;
#endif

    public:
        inline folly::CPUThreadPoolExecutor*
        getCPUExecutor()
        {
            return &poolCPU;
        }

        inline folly::IOThreadPoolExecutor*
        getIOExecutor()
        {
            return &poolIO;
        }

#if LEMON_FORCE_SINGLE_THREADED
        inline folly::ManualExecutor*
        getDebugExecutor()
        {
            return &debugExecutor;
        }
#endif

        inline folly::ManualExecutor*
        getGameThreadExecutor()
        {
#if LEMON_FORCE_SINGLE_THREADED
            return &debugExecutor;
#else
            return &gameThreadExecutor;
#endif
        }

        inline folly::ManualExecutor*
        getDrawThreadExecutor()
        {
#if LEMON_FORCE_SINGLE_THREADED
            return &debugExecutor;
#else
            return &drawThreadExecutor;
#endif
        }

        inline folly::ManualExecutor*
        getRenderThreadExecutor()
        {
#if LEMON_FORCE_SINGLE_THREADED
            return &debugExecutor;
#else
            return &renderThreadExecutor;
#endif
        }

        void
        processGameThreadTasks();

        static std::optional<std::string>
        getThreadName();

        template<typename TResult>
        inline TResult
        block(folly::Future<TResult>&& awaitable)
        {
            return folly::coro::blockingWait(std::move(awaitable));
        }
    };

    namespace detail {
        template<typename TTask, typename TFuture, bool bWithPriority>
        inline TFuture
        ScheduleTaskImpl(TTask&& task, folly::Executor::KeepAlive<> executor,
                         Priority priority = Priority::Medium)
        {
#if LEMON_FORCE_SINGLE_THREADED
            auto* debugExecutor = Scheduler::get()->getDebugExecutor();
            auto result = std::move(task).semi().via(folly::Executor::getKeepAliveToken(debugExecutor));
            debugExecutor->drain();
            return result;
#else
            if constexpr (bWithPriority) {
                return std::move(task).semi().via(executor, static_cast<int8_t>(priority));
            } else {
                return std::move(task).semi().via(executor);
            }
#endif
        }
    } // namespace detail

    template<typename TResult, typename TError>
    TaskFuture<TResult, TError>
    runIOTask(Task<TResult, TError>&& task)
    {
        return detail::ScheduleTaskImpl<Task<TResult, TError>, TaskFuture<TResult, TError>, false>(
            std::move(task), Scheduler::get()->getIOExecutor()->weakRef());
    }

    template<typename TResult, typename TError>
    TaskFuture<TResult, TError>
    runCPUTask(Task<TResult, TError>&& task, Priority priority = Priority::Medium)
    {
        return detail::ScheduleTaskImpl<Task<TResult, TError>, TaskFuture<TResult, TError>, true>(
            std::move(task), Scheduler::get()->getCPUExecutor()->weakRef(), priority);
    }

    template<typename TResult, typename TError>
    TaskFuture<TResult, TError>
    runDrawTask(Task<TResult, TError>&& task)
    {
        auto* executor = Scheduler::get()->getDrawThreadExecutor();
        auto kaExecutor = folly::Executor::getKeepAliveToken(executor);
        return detail::ScheduleTaskImpl<Task<TResult, TError>, TaskFuture<TResult, TError>, false>(
            std::move(task), kaExecutor);
    }

    template<typename TResult, typename TError>
    TaskFuture<TResult, TError>
    runRenderTask(Task<TResult, TError>&& task)
    {
        auto* executor = Scheduler::get()->getRenderThreadExecutor();
        auto kaExecutor = folly::Executor::getKeepAliveToken(executor);
        return detail::ScheduleTaskImpl<Task<TResult, TError>, TaskFuture<TResult, TError>, false>(
            std::move(task), kaExecutor);
    }

    template<typename TError>
    VoidTaskFuture<TError>
    runIOTask(VoidTask<TError>&& task)
    {
        return detail::ScheduleTaskImpl<VoidTask<TError>, VoidTaskFuture<TError>, false>(
            std::move(task), Scheduler::get()->getIOExecutor()->weakRef());
    }

    template<typename TError>
    VoidTaskFuture<TError>
    runCPUTask(VoidTask<TError>&& task, Priority priority = Priority::Medium)
    {
        return detail::ScheduleTaskImpl<VoidTask<TError>, VoidTaskFuture<TError>, true>(
            std::move(task), Scheduler::get()->getCPUExecutor()->weakRef(), priority);
    }

    template<typename TError>
    VoidTaskFuture<TError>
    runDrawTask(VoidTask<TError>&& task)
    {
        auto* executor = Scheduler::get()->getDrawThreadExecutor();
        auto kaExecutor = folly::Executor::getKeepAliveToken(executor);
        return detail::ScheduleTaskImpl<VoidTask<TError>, VoidTaskFuture<TError>, false>(std::move(task),
                                                                                         kaExecutor);
    }

    template<typename TError>
    VoidTaskFuture<TError>
    runRenderTask(VoidTask<TError>&& task)
    {
        auto* executor = Scheduler::get()->getRenderThreadExecutor();
        auto kaExecutor = folly::Executor::getKeepAliveToken(executor);
        return detail::ScheduleTaskImpl<VoidTask<TError>, VoidTaskFuture<TError>, false>(std::move(task),
                                                                                         kaExecutor);
    }
} // namespace lemon::scheduler
