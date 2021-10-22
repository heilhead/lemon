#pragma once

namespace lemon {
    using DelegateHandle = uint64_t;

    template<typename TResult, typename... TArgs>
    class DelegateBase : public NonCopyable {
        static inline std::atomic<DelegateHandle> handleCounter = 0;

        DelegateHandle handle;

    public:
        using Result = TResult;

        DelegateBase();

        virtual ~DelegateBase() {}

        virtual TResult
        execute(TArgs&&... args) = 0;

        inline DelegateHandle
        getHandle() const;
    };

    template<typename TResult, typename... TArgs>
    class LambdaDelegate : public DelegateBase<TResult, TArgs...> {
    public:
        using Function = std::function<TResult(TArgs...)>;

    private:
        Function func;

    public:
        LambdaDelegate(Function func);

        TResult
        execute(TArgs&&... args);
    };

    template<typename TResult, typename... TArgs>
    class MemberDelegate : public LambdaDelegate<TResult, TArgs...> {
    public:
        template<class T>
        using MemberPtr = TResult (T::*)(TArgs...);

        template<class T>
        MemberDelegate(MemberPtr<T> member, T* ctx);
    };

    template<typename TResult, typename... TArgs>
    class FunctionDelegate : public DelegateBase<TResult, TArgs...> {
    public:
        using Function = TResult (*)(TArgs...);

    private:
        Function func;

    public:
        FunctionDelegate(Function func);

        TResult
        execute(TArgs&&... args) final;
    };

    template<size_t Size, typename TResult, typename... TArgs>
    using DelegateVec = folly::small_vector<std::unique_ptr<DelegateBase<TResult, TArgs...>>, Size>;

    struct DelegateForwardExecutionPolicy {
        template<size_t Size, typename TResult, typename... TArgs>
        inline void
        execute(const DelegateVec<Size, TResult, TArgs...>& delegates, TArgs&&... args);
    };

    struct DelegateReverseExecutionPolicy {
        template<size_t Size, typename TResult, typename... TArgs>
        void
        execute(const DelegateVec<Size, TResult, TArgs...>& delegates, TArgs&&... args);
    };

    template<typename ExecutionPolicy, typename TResult, typename... TArgs>
    class DelegateStackBase {
        static constexpr size_t kDefaultStackSize = 8;

        DelegateVec<kDefaultStackSize, TResult, TArgs...> delegates;

    public:
        using LambdaDelegate = LambdaDelegate<TResult, TArgs...>;
        using MemberDelegate = MemberDelegate<TResult, TArgs...>;
        using FunctionDelegate = FunctionDelegate<TResult, TArgs...>;

        DelegateStackBase() = default;

        [[nodiscard]] DelegateHandle
        add(LambdaDelegate::Function func);

        template<class T>
        [[nodiscard]] DelegateHandle
        add(MemberDelegate::template MemberPtr<T> member, T* ctx);

        [[nodiscard]] DelegateHandle
        add(FunctionDelegate::Function fn);

        void
        execute(TArgs&&... args);

        bool
        remove(DelegateHandle handle);
    };

    template<typename... Args>
    using DelegateStack = DelegateStackBase<DelegateForwardExecutionPolicy, void, Args...>;

    template<typename... Args>
    using ReverseDelegateStack = DelegateStackBase<DelegateReverseExecutionPolicy, void, Args...>;
} // namespace lemon

#include <lemon/misc/Delegate.inl>
