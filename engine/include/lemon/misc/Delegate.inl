#pragma once

#include <range/v3/view/reverse.hpp>

template<typename TResult, typename... TArgs>
lemon::DelegateHandle
lemon::DelegateHandleProducer<TResult, TArgs...>::create()
{
    // TODO: Care about overflowing?
    return ++counter;
}

template<typename TResult, typename... TArgs>
template<typename TBoundable>
lemon::Delegate<TResult, TArgs...>::Delegate(TBoundable&& boundable)
    : func{std::forward<TBoundable>(boundable)}
{
}

template<typename TResult, typename... TArgs>
template<class TClass>
lemon::Delegate<TResult, TArgs...>::Delegate(MemberPtr<TClass> member, TClass* instance) : func{}
{
    bind(member, instance);
}

template<typename TResult, typename... TArgs>
template<typename... TBindArgs>
void
lemon::Delegate<TResult, TArgs...>::set(TBindArgs&&... args)
{
    func = std::function<TResult(TArgs...)>(std::forward<TBindArgs>(args)...);
}

template<typename TResult, typename... TArgs>
template<typename... TBindArgs>
void
lemon::Delegate<TResult, TArgs...>::bind(TBindArgs&&... args)
{
    func = std::bind_front(std::forward<TBindArgs>(args)...);
}

template<typename TResult, typename... TArgs>
inline void
lemon::Delegate<TResult, TArgs...>::clear()
{
    func = nullptr;
}

template<typename TResult, typename... TArgs>
template<typename... TInvocationArgs>
inline TResult
lemon::Delegate<TResult, TArgs...>::invoke(TInvocationArgs&&... args) const
{
    return func(std::forward<TInvocationArgs>(args)...);
}

template<typename TResult, typename... TArgs>
inline bool
lemon::Delegate<TResult, TArgs...>::isValid() const
{
    return (bool)func;
}

template<typename TResult, typename... TArgs>
template<typename... TInvocationArgs>
inline TResult
lemon::Delegate<TResult, TArgs...>::operator()(TInvocationArgs&&... args) const
{
    return invoke(std::forward<TInvocationArgs>(args)...);
}

template<typename TResult, typename... TArgs>
inline lemon::Delegate<TResult, TArgs...>::operator bool() const
{
    return isValid();
}

template<size_t Size, typename TResult, typename... TArgs>
inline void
lemon::DelegateForwardExecutionPolicy::invoke(const DelegateVec<Size, TResult, TArgs...>& delegates,
                                              TArgs&&... args)
{
    for (auto& d : delegates) {
        d.second.invoke(std::forward<TArgs>(args)...);
    }
}

template<size_t Size, typename TResult, typename... TArgs>
inline void
lemon::DelegateReverseExecutionPolicy::invoke(const DelegateVec<Size, TResult, TArgs...>& delegates,
                                              TArgs&&... args)
{
    for (auto& d : ranges::reverse_view(delegates)) {
        d.second.invoke(std::forward<TArgs>(args)...);
    }
}

template<typename ExecutionPolicy, typename TResult, typename... TArgs>
template<typename TBoundable>
lemon::DelegateHandle
lemon::MulticastDelegateBase<ExecutionPolicy, TResult, TArgs...>::add(TBoundable&& boundable)
{
    const auto handle = createHandle();
    delegates.emplace_back(
        std::make_pair(handle, Delegate<TResult, TArgs...>(std::forward<TBoundable>(boundable))));
    return handle;
}

template<typename ExecutionPolicy, typename TResult, typename... TArgs>
template<class TClass>
lemon::DelegateHandle
lemon::MulticastDelegateBase<ExecutionPolicy, TResult, TArgs...>::add(
    ConcreteDelegate::template MemberPtr<TClass> member, TClass* instance)
{
    const auto handle = createHandle();
    delegates.emplace_back(std::make_pair(handle, Delegate<TResult, TArgs...>(member, instance)));
    return handle;
}

template<typename ExecutionPolicy, typename TResult, typename... TArgs>
template<typename... TInvocationArgs>
inline void
lemon::MulticastDelegateBase<ExecutionPolicy, TResult, TArgs...>::invoke(TInvocationArgs&&... args)
{
    ExecutionPolicy().invoke(delegates, std::forward<TArgs>(args)...);
}

template<typename ExecutionPolicy, typename TResult, typename... TArgs>
bool
lemon::MulticastDelegateBase<ExecutionPolicy, TResult, TArgs...>::remove(DelegateHandle handle)
{
    int idx = kInvalidIndex;
    for (int i = 0; i < delegates.size(); i++) {
        if (delegates[i].first == handle) {
            idx = i;
            break;
        }
    }

    if (idx != kInvalidIndex) {
        delegates.erase(delegates.begin() + idx);
        return true;
    }

    return false;
}

template<typename ExecutionPolicy, typename TResult, typename... TArgs>
template<typename... TInvocationArgs>
inline void
lemon::MulticastDelegateBase<ExecutionPolicy, TResult, TArgs...>::operator()(TInvocationArgs&&... args)
{
    invoke(std::forward<TInvocationArgs>(args)...);
}

template<typename ExecutionPolicy, typename TResult, typename... TArgs>
inline lemon::DelegateHandle
lemon::MulticastDelegateBase<ExecutionPolicy, TResult, TArgs...>::createHandle() const
{
    return DelegateHandleProducer<TResult, TArgs...>::create();
}
