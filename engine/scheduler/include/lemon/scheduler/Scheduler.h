#pragma once

#include <optional>
#include <string>
#include <lemon/scheduler/prelude.h>
#include <folly/executors/IOThreadPoolExecutor.h>
#include <folly/executors/CPUThreadPoolExecutor.h>
#include <folly/experimental/coro/Task.h>

namespace lemon::scheduler {
    template<typename TResult, typename TError>
    using Task = folly::coro::Task<tl::expected<TResult, TError>>;

    template<typename TResult, typename TError>
    using TaskFuture = folly::Future<tl::expected<TResult, TError>>;

    template<typename TError>
    using VoidTask = folly::coro::Task<std::optional<TError>>;

    template<typename TError>
    using VoidTaskFuture = folly::Future<std::optional<TError>>;

    enum class Priority {
        Low = folly::Executor::LO_PRI,
        Medium = folly::Executor::MID_PRI,
        High = folly::Executor::HI_PRI
    };

    class Scheduler {
    public:
        Scheduler(size_t threadsIO = std::thread::hardware_concurrency(),
                  size_t threadsCPU = std::thread::hardware_concurrency());
        ~Scheduler();

        static Scheduler*
        get();

    private:
        std::unique_ptr<folly::IOThreadPoolExecutor> poolIO;
        std::unique_ptr<folly::CPUThreadPoolExecutor> poolCPU;

    public:
        inline folly::CPUThreadPoolExecutor*
        getCPUExecutor() {
            return poolCPU.get();
        }

        inline folly::IOThreadPoolExecutor*
        getIOExecutor() {
            return poolIO.get();
        }

        static std::optional<std::string>
        getCurrentThreadName();
    };

    namespace detail {
        template<typename TTask, typename TFuture, bool bWithPriority>
        inline TFuture
        ScheduleTaskImpl(TTask&& task, folly::Executor::KeepAlive<> executor,
                         Priority priority = Priority::Medium) {
            if constexpr (bWithPriority) {
                return std::move(task).semi().via(executor, static_cast<int8_t>(priority));
            } else {
                return std::move(task).semi().via(executor);
            }
        }
    } // namespace detail

    template<typename TResult, typename TError>
    TaskFuture<TResult, TError>
    IOTask(Task<TResult, TError>&& task) {
        return detail::ScheduleTaskImpl<Task<TResult, TError>, TaskFuture<TResult, TError>, false>(
            std::move(task), Scheduler::get()->getIOExecutor()->weakRef());
    }

    template<typename TResult, typename TError>
    TaskFuture<TResult, TError>
    CPUTask(Task<TResult, TError>&& task, Priority priority = Priority::Medium) {
        return detail::ScheduleTaskImpl<Task<TResult, TError>, TaskFuture<TResult, TError>, true>(
            std::move(task), Scheduler::get()->getCPUExecutor()->weakRef(), priority);
    }

    template<typename TError>
    VoidTaskFuture<TError>
    IOTask(VoidTask<TError>&& task) {
        return detail::ScheduleTaskImpl<VoidTask<TError>, VoidTaskFuture<TError>, false>(
            std::move(task), Scheduler::get()->getIOExecutor()->weakRef());
    }

    template<typename TError>
    VoidTaskFuture<TError>
    CPUTask(VoidTask<TError>&& task, Priority priority = Priority::Medium) {
        return detail::ScheduleTaskImpl<VoidTask<TError>, VoidTaskFuture<TError>, true>(
            std::move(task), Scheduler::get()->getCPUExecutor()->weakRef(), priority);
    }
} // namespace lemon::scheduler
