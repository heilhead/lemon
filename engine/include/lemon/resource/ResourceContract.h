#pragma once

#include <folly/experimental/coro/Baton.h>
#include <lemon/resource/common.h>
#include <lemon/scheduler.h>
#include <optional>
#include <tl/expected.hpp>

namespace lemon::res {
    class ResourceInstance;

    enum class ResourceLifetime : int8_t {
        Static = -1,
        None = 0,
        Short = 1,
        Medium = 3,
        Long = 5,
    };

    struct ResourceContract {
        friend class ResourceStore;

        template<typename T>
        using ResolutionType = tl::expected<T*, ResourceLoadingError>;

        template<typename T>
        using PromiseType = folly::Promise<ResolutionType<T>>;

        template<typename T>
        using SemiFutureType = folly::SemiFuture<ResolutionType<T>>;

        template<typename T>
        using FutureType = folly::Future<ResolutionType<T>>;

    public:
        explicit ResourceContract();
        ~ResourceContract();

    private:
        PromiseType<void> promise{PromiseType<void>::makeEmpty()};
        FutureType<void> future{FutureType<void>::makeEmpty()};
        folly::coro::Baton baton;
        int8_t lifetime{(int8_t)ResourceLifetime::Short};

    public:
        template<typename T = void>
        inline PromiseType<T>&
        getPromise() {
            return reinterpret_cast<PromiseType<T>&>(promise);
        }

        template<typename T = void>
        inline SemiFutureType<T>&
        getFuture() {
            return reinterpret_cast<SemiFutureType<T>&>(future);
        }

        inline folly::coro::Baton&
        getBaton() {
            return baton;
        }

        inline void
        setLifetime(ResourceLifetime inLifetime) {
            lifetime = (int8_t)inLifetime;
        }
    };
} // namespace lemon::res