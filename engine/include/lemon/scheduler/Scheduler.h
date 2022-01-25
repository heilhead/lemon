#pragma once

#include <lemon/scheduler/common.h>
#include <lemon/profiling.h>
#include <lemon/portability.h>

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

    enum class TaskExecutorType { GameThread, DrawThread, RenderThread, CPUThreadPool, IOThreadPool };

    class Scheduler : public UnsafeSingleton<Scheduler> {
    public:
#if LEMON_ENABLE_THREADING
        Scheduler(size_t numCPUThreads = std::thread::hardware_concurrency(),
                  size_t numIOThreads = std::thread::hardware_concurrency());
#else
        Scheduler(size_t numCPUThreads = 0, size_t numIOThreads = 0);
#endif

    private:
        folly::ManualExecutor gameThreadExecutor;

#if LEMON_ENABLE_THREADING
        folly::ManualExecutor drawThreadExecutor;
        folly::ManualExecutor renderThreadExecutor;
        folly::IOThreadPoolExecutor IOPoolExecutor;
        folly::CPUThreadPoolExecutor CPUPoolExecutor;
        std::thread drawThread;
        std::thread renderThread;
#endif

    public:
        folly::Executor*
        getExecutor(TaskExecutorType type);

        inline folly::Executor::KeepAlive<folly::Executor>
        getExecutorToken(TaskExecutorType type)
        {
            auto* executor = getExecutor(type);
            return folly::Executor::getKeepAliveToken(executor);
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

        template<typename TResult>
        folly::Future<TResult>
        scheduleTask(folly::coro::Task<TResult>&& task,
                     TaskExecutorType type = TaskExecutorType::CPUThreadPool);
    };

    template<typename TTask, typename TFuture, bool bWithPriority>
    inline TFuture
    scheduleTaskImpl(TTask&& task, folly::Executor::KeepAlive<> executor,
                     Priority priority = Priority::Medium)
    {
#if LEMON_ENABLE_THREADING
        if constexpr (bWithPriority) {
            return std::move(task).semi().via(executor, static_cast<int8_t>(priority));
        } else {
            return std::move(task).semi().via(executor);
        }
#else
        auto result = std::move(task).semi().via(executor);
        dynamic_cast<folly::ManualExecutor*>(Scheduler::get()->getExecutor(TaskExecutorType::GameThread))
            ->drain();
        return result;
#endif
    }

    template<typename TResult>
    inline folly::Future<TResult>
    scheduleTask(folly::coro::Task<TResult>&& task, TaskExecutorType type = TaskExecutorType::CPUThreadPool)
    {
        return Scheduler::get()->scheduleTask(std::move(task), type);
    }

    template<typename TResult>
    inline folly::Future<TResult>
    Scheduler::scheduleTask(folly::coro::Task<TResult>&& task, TaskExecutorType type)
    {
        return scheduleTaskImpl<folly::coro::Task<TResult>, folly::Future<TResult>, false>(
            std::move(task), getExecutorToken(type));
    }

    template<typename TResult, typename TError>
    inline TaskFuture<TResult, TError>
    runGameThreadTask(Task<TResult, TError>&& task)
    {
        return scheduleTaskImpl<Task<TResult, TError>, TaskFuture<TResult, TError>, false>(
            std::move(task), Scheduler::get()->getExecutorToken(TaskExecutorType::GameThread));
    }

    template<typename TResult, typename TError>
    inline TaskFuture<TResult, TError>
    runDrawThreadTask(Task<TResult, TError>&& task)
    {
        return scheduleTaskImpl<Task<TResult, TError>, TaskFuture<TResult, TError>, false>(
            std::move(task), Scheduler::get()->getExecutorToken(TaskExecutorType::DrawThread));
    }

    template<typename TResult, typename TError>
    inline TaskFuture<TResult, TError>
    runRenderThreadTask(Task<TResult, TError>&& task)
    {
        return scheduleTaskImpl<Task<TResult, TError>, TaskFuture<TResult, TError>, false>(
            std::move(task), Scheduler::get()->getExecutorToken(TaskExecutorType::RenderThread));
    }

    template<typename TResult, typename TError>
    inline TaskFuture<TResult, TError>
    runCPUThreadTask(Task<TResult, TError>&& task, Priority priority = Priority::Medium)
    {
        return scheduleTaskImpl<Task<TResult, TError>, TaskFuture<TResult, TError>, true>(
            std::move(task), Scheduler::get()->getExecutorToken(TaskExecutorType::CPUThreadPool), priority);
    }

    template<typename TResult, typename TError>
    inline TaskFuture<TResult, TError>
    runIOThreadTask(Task<TResult, TError>&& task)
    {
        return scheduleTaskImpl<Task<TResult, TError>, TaskFuture<TResult, TError>, false>(
            std::move(task), Scheduler::get()->getExecutorToken(TaskExecutorType::IOThreadPool));
    }

    template<typename TError>
    inline VoidTaskFuture<TError>
    runGameThreadTask(VoidTask<TError>&& task)
    {
        return scheduleTaskImpl<VoidTask<TError>, VoidTaskFuture<TError>, false>(
            std::move(task), Scheduler::get()->getExecutorToken(TaskExecutorType::GameThread));
    }

    template<typename TError>
    inline VoidTaskFuture<TError>
    runDrawThreadTask(VoidTask<TError>&& task)
    {
        return scheduleTaskImpl<VoidTask<TError>, VoidTaskFuture<TError>, false>(
            std::move(task), Scheduler::get()->getExecutorToken(TaskExecutorType::DrawThread));
    }

    template<typename TError>
    inline VoidTaskFuture<TError>
    runRenderThreadTask(VoidTask<TError>&& task)
    {
        return scheduleTaskImpl<VoidTask<TError>, VoidTaskFuture<TError>, false>(
            std::move(task), Scheduler::get()->getExecutorToken(TaskExecutorType::RenderThread));
    }

    template<typename TError>
    inline VoidTaskFuture<TError>
    runCPUThreadTask(VoidTask<TError>&& task, Priority priority = Priority::Medium)
    {
        return scheduleTaskImpl<VoidTask<TError>, VoidTaskFuture<TError>, true>(
            std::move(task), Scheduler::get()->getExecutorToken(TaskExecutorType::CPUThreadPool), priority);
    }

    template<typename TError>
    inline VoidTaskFuture<TError>
    runIOThreadTask(VoidTask<TError>&& task)
    {
        return scheduleTaskImpl<VoidTask<TError>, VoidTaskFuture<TError>, false>(
            std::move(task), Scheduler::get()->getExecutorToken(TaskExecutorType::IOThreadPool));
    }

    template<typename TElement, typename TFutureData = folly::Unit>
    folly::coro::Task<std::vector<TFutureData>>
    coForEach(std::vector<TElement>& elements, std::function<folly::coro::Task<TFutureData>(TElement&)> func)
    {
        const folly::Executor::KeepAlive<> kaExecutor = co_await folly::coro::co_current_executor;
        std::vector<folly::Future<TFutureData>> futs;
        futs.reserve(elements.size());

        for (auto& element : elements) {
            futs.emplace_back(
                scheduleTaskImpl<folly::coro::Task<TFutureData>, folly::Future<TFutureData>, false>(
                    func(element), kaExecutor));
        }

        co_return co_await folly::coro::collectAllRange(futs | ranges::views::move);
    }
} // namespace lemon::scheduler
