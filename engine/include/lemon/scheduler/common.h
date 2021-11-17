#pragma once

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
} // namespace lemon::scheduler
