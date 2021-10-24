#pragma once

namespace lemon {
    using DelegateHandle = uint64_t;

    template<typename TResult, typename... TArgs>
    struct DelegateHandleProducer {
        static DelegateHandle
        create();

    private:
        static inline std::atomic<DelegateHandle> counter = 0;
    };

    template<typename TResult, typename... TArgs>
    class Delegate {
        std::function<TResult(TArgs...)> func;

    public:
        template<class T>
        using MemberPtr = TResult (T::*)(TArgs...);

        Delegate() = default;

        template<typename TBoundable>
        Delegate(TBoundable&& boundable);

        template<class TClass>
        Delegate(MemberPtr<TClass> member, TClass* instance);

        template<typename... TBindArgs>
        void
        set(TBindArgs&&... args);

        template<typename... TBindArgs>
        void
        bind(TBindArgs&&... args);

        void
        clear();

        template<typename... TInvocationArgs>
        TResult
        invoke(TInvocationArgs&&... args) const;

        bool
        isValid() const;

        template<typename... TInvocationArgs>
        TResult
        operator()(TInvocationArgs&&... args) const;

        operator bool() const;
    };

    template<size_t Size, typename TResult, typename... TArgs>
    using DelegateVec = folly::small_vector<std::pair<DelegateHandle, Delegate<TResult, TArgs...>>, Size>;

    struct DelegateForwardExecutionPolicy {
        template<size_t Size, typename TResult, typename... TArgs>
        inline void
        invoke(const DelegateVec<Size, TResult, TArgs...>& delegates, TArgs&&... args);
    };

    struct DelegateReverseExecutionPolicy {
        template<size_t Size, typename TResult, typename... TArgs>
        void
        invoke(const DelegateVec<Size, TResult, TArgs...>& delegates, TArgs&&... args);
    };

    template<typename ExecutionPolicy, typename TResult, typename... TArgs>
    class MulticastDelegateBase {
        static constexpr size_t kDefaultStackSize = 6;

        DelegateVec<kDefaultStackSize, TResult, TArgs...> delegates;

    public:
        using ConcreteDelegate = Delegate<TResult, TArgs...>;

        MulticastDelegateBase() = default;

        template<typename TBoundable>
        [[nodiscard]] DelegateHandle
        add(TBoundable&& boundable);

        template<class TClass>
        [[nodiscard]] DelegateHandle
        add(ConcreteDelegate::template MemberPtr<TClass> member, TClass* instance);

        template<typename... TInvocationArgs>
        void
        invoke(TInvocationArgs&&... args);

        bool
        remove(DelegateHandle handle);

        template<typename... TInvocationArgs>
        void
        operator()(TInvocationArgs&&... args);

    private:
        DelegateHandle
        createHandle() const;
    };

    template<typename... TArgs>
    using MulticastDelegate = MulticastDelegateBase<DelegateForwardExecutionPolicy, void, TArgs...>;

    template<typename... TArgs>
    using ReverseMulticastDelegate = MulticastDelegateBase<DelegateReverseExecutionPolicy, void, TArgs...>;
} // namespace lemon

#include <lemon/misc/Delegate.inl>
