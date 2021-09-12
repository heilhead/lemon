#pragma once

#include <optional>
#include <tl/expected.hpp>
#include <lemon/resource/common.h>
#include <lemon/scheduler.h>
#include <folly/experimental/coro/Baton.h>

namespace lemon::res {
    class ResourceInstance;

    enum class ResourceLifetime {
        Static,
        Short,
        Medium,
        Long,
    };

    struct ResourceContract {
        template<typename T>
        using ResolutionType = tl::expected<T*, ResourceLoadingError>;

        template<typename T>
        using PromiseType = folly::Promise<ResolutionType<T>>;

        template<typename T>
        using SemiFutureType = folly::SemiFuture<ResolutionType<T>>;

        template<typename T>
        using FutureType = folly::Future<ResolutionType<T>>;

    public:
        explicit ResourceContract(ResourceLifetime lifetime = ResourceLifetime::Short);
        ~ResourceContract();

    private:
        PromiseType<void> promise { PromiseType<void>::makeEmpty() };
        FutureType<void> future { FutureType<void>::makeEmpty() };
        folly::coro::Baton baton;

    public:
        template<typename T = void>
        inline PromiseType<T>& getPromise() {
            return reinterpret_cast<PromiseType<T>&>(promise);
        }

        template<typename T = void>
        inline SemiFutureType<T>& getFuture() {
            return reinterpret_cast<SemiFutureType<T>&>(future);
        }

        inline folly::coro::Baton& getBaton() {
            return baton;
        }
    };
}