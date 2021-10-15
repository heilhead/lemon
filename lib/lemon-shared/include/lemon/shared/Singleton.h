#pragma once

#include <mutex>
#include <lemon/shared/NonCopyable.h>

namespace lemon {
    /// <summary>
    /// Inspired by Canonical's multipass:
    /// https://github.com/canonical/multipass
    /// </summary>
    /// <typeparam name="T"></typeparam>
    template<typename T>
    class PrivatePassProvider {
    public:
        virtual ~PrivatePassProvider() = default;

        class PrivatePass {
        private:
            constexpr PrivatePass() = default;
            friend class PrivatePassProvider<T>;
        };

    private:
        static constexpr const PrivatePass pass{};
        friend T;
    };

    /// <summary>
    /// Thread-safe version of singleton. Derived class' constructor can be public, but must require the
    /// private pass token.
    /// </summary>
    /// <typeparam name="TDerived"></typeparam>
    template<class TDerived>
    class Singleton : public PrivatePassProvider<Singleton<TDerived>>, NonMovable {
        using Base = PrivatePassProvider<Singleton<TDerived>>;

        // N.B. `std::once_flag` is not assignable, so wrap it in a `std::unique_ptr` so that we can reset it
        // later.
        static std::unique_ptr<std::once_flag> singletonFlag;
        static std::unique_ptr<TDerived> singleton;

    public:
        using Pass = typename Base::PrivatePass;

        constexpr Singleton(const Pass&) noexcept {}
        virtual ~Singleton() = default;

        static inline TDerived&
        get() noexcept(noexcept(TDerived(Base::pass)))
        {
            std::call_once(*singletonFlag, [] { singleton = std::make_unique<TDerived>(Base::pass); });
            return *singleton;
        }

        /// <summary>
        /// Note: not thread-safe.
        /// </summary>
        /// <returns></returns>
        static inline TDerived*
        getUnchecked() noexcept
        {
            return singleton.get();
        }

        /// <summary>
        /// Note: not thread-safe.
        /// </summary>
        /// <returns></returns>
        static inline void
        reset() noexcept
        {
            singletonFlag = std::make_unique<std::once_flag>();
            singleton.reset(nullptr);
        }
    };

    template<class TDerived>
    std::unique_ptr<std::once_flag> Singleton<TDerived>::singletonFlag = std::make_unique<std::once_flag>();

    template<class TDerived>
    std::unique_ptr<TDerived> Singleton<TDerived>::singleton = nullptr;
} // namespace lemon
