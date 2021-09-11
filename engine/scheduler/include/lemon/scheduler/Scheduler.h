#pragma once

#include <lemon/scheduler/prelude.h>
#include <folly/executors/IOThreadPoolExecutor.h>
#include <folly/executors/CPUThreadPoolExecutor.h>
#include <folly/experimental/coro/Task.h>

namespace lemon::scheduler {
    template<typename T = void, typename U = int>
    using Task = folly::coro::Task<tl::expected<T, U>>;

    template<typename T = void, typename U = int>
    using TaskFuture = folly::Future<tl::expected<T, U>>;

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

        static Scheduler* get();

    private:
        std::unique_ptr<folly::IOThreadPoolExecutor> poolIO;
        std::unique_ptr<folly::CPUThreadPoolExecutor> poolCPU;

    public:
        inline folly::CPUThreadPoolExecutor* getCPUExecutor() {
            return poolCPU.get();
        }

        inline folly::IOThreadPoolExecutor* getIOExecutor() {
            return poolIO.get();
        }
    };

    template<typename T, typename U>
    TaskFuture<T, U> IOTask(Task<T, U>&& task) {
        return std::move(task).semi().via(Scheduler::get()->getIOExecutor()->weakRef());
    }

    template<typename T, typename U>
    TaskFuture<T, U> CPUTask(Task<T, U>&& task, Priority priority = Priority::Medium) {
        return std::move(task).semi()
                              .via(Scheduler::get()->getCPUExecutor()->weakRef(), static_cast<int8_t>(priority));
    }
}
