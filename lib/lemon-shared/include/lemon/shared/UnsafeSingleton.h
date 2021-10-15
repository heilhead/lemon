#pragma once

#include <lemon/shared/logger.h>
#include <lemon/shared/NonCopyable.h>

namespace lemon {
    /// <summary>
    /// Thread-unsafe version of singleton. Derived class instance must be managed manually, and the instance
    /// pointer is available only for the lifetime of the global instance.
    /// </summary>
    /// <typeparam name="TDerived"></typeparam>
    template<class TDerived>
    class UnsafeSingleton : NonMovable {
        static TDerived* singleton;

    public:
        UnsafeSingleton() noexcept
        {
            LEMON_ASSERT(singleton == nullptr,
                         "only one instance a singleton class can be alive at the same time");
            singleton = static_cast<TDerived*>(this);
        }

        virtual ~UnsafeSingleton()
        {
            singleton = nullptr;
        }

        static TDerived*
        get() noexcept
        {
            return singleton;
        }
    };

    template<class TDerived>
    TDerived* UnsafeSingleton<TDerived>::singleton = nullptr;
} // namespace lemon
