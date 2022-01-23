#pragma once

#include <lemon/scheduler/common.h>

namespace lemon::scheduler {
    class Scheduler : public UnsafeSingleton<Scheduler> {
    public:
        Scheduler(size_t threadsIO = std::thread::hardware_concurrency(),
                  size_t threadsCPU = std::thread::hardware_concurrency());

    private:
        folly::IOThreadPoolExecutor poolIO;
        folly::CPUThreadPoolExecutor poolCPU;

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
        folly::ManualExecutor*
        getDebugExecutor()
        {
            return &debugExecutor;
        }
#endif

        static std::optional<std::string>
        getCurrentThreadName();

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
    IOTask(Task<TResult, TError>&& task)
    {
        return detail::ScheduleTaskImpl<Task<TResult, TError>, TaskFuture<TResult, TError>, false>(
            std::move(task), Scheduler::get()->getIOExecutor()->weakRef());
    }

    template<typename TResult, typename TError>
    TaskFuture<TResult, TError>
    CPUTask(Task<TResult, TError>&& task, Priority priority = Priority::Medium)
    {
        return detail::ScheduleTaskImpl<Task<TResult, TError>, TaskFuture<TResult, TError>, true>(
            std::move(task), Scheduler::get()->getCPUExecutor()->weakRef(), priority);
    }

    template<typename TError>
    VoidTaskFuture<TError>
    IOTask(VoidTask<TError>&& task)
    {
        return detail::ScheduleTaskImpl<VoidTask<TError>, VoidTaskFuture<TError>, false>(
            std::move(task), Scheduler::get()->getIOExecutor()->weakRef());
    }

    template<typename TError>
    VoidTaskFuture<TError>
    CPUTask(VoidTask<TError>&& task, Priority priority = Priority::Medium)
    {
        return detail::ScheduleTaskImpl<VoidTask<TError>, VoidTaskFuture<TError>, true>(
            std::move(task), Scheduler::get()->getCPUExecutor()->weakRef(), priority);
    }
} // namespace lemon::scheduler
