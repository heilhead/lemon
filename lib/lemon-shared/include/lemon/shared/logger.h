#pragma once

#include <iostream>
#include <gsl/gsl>
#include <lemon/shared/Singleton.h>

namespace lemon::logger {
    enum class LogLevel {
        None = 0,
        Fatal = 1,
        Error = 2,
        Warning = 3,
        Log = 4,
        Trace = 5,
    };

    namespace detail {
        class Logger : public Singleton<Logger> {
            LogLevel level = LogLevel::Trace;

        public:
            Logger(const Pass& pass) : Singleton(pass) {}

            std::ostream&
            getDefaultLogStream();

            std::ostream&
            getErrorLogStream();

            void
            setLogLevel(LogLevel level);

            LogLevel
            getLogLevel();

            template<typename... Args>
            void
            printLog(std::ostream& outStream, gsl::czstring<> category, Args&&... args);

            template<typename... Args>
            void
            assertionError(gsl::czstring<> expr, gsl::czstring<> file, int line, Args&&... args);

            template<typename... Args>
            void
            unreachable(gsl::czstring<> file, int line, Args&&... args);
        };
    } // namespace detail

    void
    setLogLevel(LogLevel level);

    template<typename... Args>
    inline void
    trace(Args&&... args);

    template<typename... Args>
    inline void
    log(Args&&... args);

    template<typename... Args>
    inline void
    warn(Args&&... args);

    template<typename... Args>
    inline void
    err(Args&&... args);

    template<typename... Args>
    inline void
    fatal(Args&&... args);
} // namespace lemon::logger

#ifdef NDEBUG
#define LEMON_ASSERT(...) ((void)0)
#else
#define LEMON_ASSERT(expression, ...)                                                                        \
    (void)((!!(expression)) || (::lemon::logger::detail::Logger::get().assertionError(                       \
                                    #expression, __FILE__, __LINE__, __VA_ARGS__),                           \
                                0))
#endif

#ifdef LEMON_ENABLE_SLOW_ASSERT
#define LEMON_SLOW_ASSERT(expression, ...) LEMON_ASSERT(expression, __VA_ARGS__)
#else
#define LEMON_SLOW_ASSERT(...) ((void)0)
#endif

#define LEMON_TODO(...) ::lemon::logger::detail::Logger::get().unreachable(__FILE__, __LINE__, __VA_ARGS__)
#define LEMON_UNREACHABLE(...) LEMON_TODO(__VA_ARGS__)
#define LEMON_TRACE_FN(...) ::lemon::logger::trace(__FUNCTION__, __VA_ARGS__);

#include <lemon/shared/logger.inl>
