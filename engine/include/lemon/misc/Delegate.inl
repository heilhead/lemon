#pragma once

#include <range/v3/view/reverse.hpp>

template<typename TResult, typename... TArgs>
lemon::DelegateBase<TResult, TArgs...>::DelegateBase()
{
    // TODO: What to do in case of overflow?
    handle = ++handleCounter;
}

template<typename TResult, typename... TArgs>
inline lemon::DelegateHandle
lemon::DelegateBase<TResult, TArgs...>::getHandle() const
{
    return handle;
}

template<typename TResult, typename... TArgs>
lemon::LambdaDelegate<TResult, TArgs...>::LambdaDelegate(Function func) : func{func}
{
}

template<typename TResult, typename... TArgs>
TResult
lemon::LambdaDelegate<TResult, TArgs...>::execute(TArgs&&... args)
{
    return func(std::forward<TArgs>(args)...);
}

template<typename TResult, typename... TArgs>
template<class T>
lemon::MemberDelegate<TResult, TArgs...>::MemberDelegate(MemberPtr<T> member, T* ctx)
    : LambdaDelegate<TResult, TArgs...>(std::bind_front(std::mem_fn(member), ctx))
{
}

template<typename TResult, typename... TArgs>
lemon::FunctionDelegate<TResult, TArgs...>::FunctionDelegate(Function func) : func{func}
{
}

template<typename TResult, typename... TArgs>
TResult
lemon::FunctionDelegate<TResult, TArgs...>::execute(TArgs&&... args)
{
    return func(std::forward<TArgs>(args)...);
}

template<size_t Size, typename TResult, typename... TArgs>
inline void
lemon::DelegateForwardExecutionPolicy::execute(const DelegateVec<Size, TResult, TArgs...>& delegates,
                                               TArgs&&... args)
{
    for (auto& d : delegates) {
        d->execute(std::forward<TArgs>(args)...);
    }
}

template<size_t Size, typename TResult, typename... TArgs>
inline void
lemon::DelegateReverseExecutionPolicy::execute(const DelegateVec<Size, TResult, TArgs...>& delegates,
                                               TArgs&&... args)
{
    for (auto& d : ranges::reverse_view(delegates)) {
        d->execute(std::forward<TArgs>(args)...);
    }
}

template<typename ExecutionPolicy, typename TResult, typename... TArgs>
lemon::DelegateHandle
lemon::DelegateStackBase<ExecutionPolicy, TResult, TArgs...>::add(LambdaDelegate::Function func)
{
    auto d = std::make_unique<LambdaDelegate>(func);
    const auto handle = d->getHandle();
    delegates.emplace_back(std::move(d));
    return handle;
}

template<typename ExecutionPolicy, typename TResult, typename... TArgs>
template<class T>
lemon::DelegateHandle
lemon::DelegateStackBase<ExecutionPolicy, TResult, TArgs...>::add(
    MemberDelegate::template MemberPtr<T> member, T* ctx)
{
    auto d = std::make_unique<MemberDelegate>(member, ctx);
    const auto handle = d->getHandle();
    delegates.emplace_back(std::move(d));
    return handle;
}

template<typename ExecutionPolicy, typename TResult, typename... TArgs>
lemon::DelegateHandle
lemon::DelegateStackBase<ExecutionPolicy, TResult, TArgs...>::add(FunctionDelegate::Function fn)
{
    auto d = std::make_unique<FunctionDelegate>(fn);
    const auto handle = d->getHandle();
    delegates.emplace_back(std::move(d));
    return handle;
}

template<typename ExecutionPolicy, typename TResult, typename... TArgs>
inline void
lemon::DelegateStackBase<ExecutionPolicy, TResult, TArgs...>::execute(TArgs&&... args)
{
    ExecutionPolicy().execute(delegates, std::forward<TArgs>(args)...);
}

template<typename ExecutionPolicy, typename TResult, typename... TArgs>
bool
lemon::DelegateStackBase<ExecutionPolicy, TResult, TArgs...>::remove(DelegateHandle handle)
{
    int idx = kIndexNone;
    for (int i = 0; i < delegates.size(); i++) {
        if (delegates[i]->getHandle() == handle) {
            idx = i;
            break;
        }
    }

    if (idx != kIndexNone) {
        delegates.erase(delegates.begin() + idx);
        return true;
    }

    return false;
}
