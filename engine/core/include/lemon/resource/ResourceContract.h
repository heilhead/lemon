#pragma once

#include <lemon/scheduler.h>

namespace lemon::res {
    enum class ResourceLifetime {
        Static,
        Short,
        Medium,
        Long,
    };

    struct ResourceContract {
    public:
        explicit ResourceContract(ResourceLifetime lifetime = ResourceLifetime::Short);
        ~ResourceContract();

    private:
        std::pair<folly::Promise<void*>, folly::SemiFuture<void*>> contract;

    public:
        template<typename T = void>
        folly::Promise<T*>* getPromise() {
            return &contract.first;
        }

        template<typename T = void>
        folly::SemiFuture<T*>* getFuture() {
            return &contract.second;
        }
    };
}