#pragma once

#include <ctime>
#include <cstdio>
#include <sstream>
#include <iostream>
#include <iomanip>

namespace lemon::logger {
    namespace detail {
        inline std::ostream&
        getDefaultLogStream()
        {
            return std::cout;
        }

        inline std::ostream&
        getErrorLogStream()
        {
            return std::cerr;
        }

        template<typename... Args>
        void
        printLog(std::ostream& outStream, const char* category, Args&&... args)
        {
            auto now = std::time(nullptr);
            auto* localNow = std::localtime(&now);

            std::stringstream stream;
            stream << "[" << std::put_time(localNow, "%F %T") << "] ";
            stream << "[" << category << "] ";

            (stream << ... << args) << std::endl;

            outStream << stream.view();
        }

        template<typename... Args>
        inline void
        assertionError(const char* expr, const char* file, int line, Args&&... args)
        {
            std::stringstream stream;
            stream << "Assertion `" << expr << "` failed in " << file << " line " << line;

            if constexpr (sizeof...(Args) > 0) {
                stream << ": ";
                (stream << ... << args);
            }

            printLog(getErrorLogStream(), "ASSERTION ERROR", stream.view());
        }
    } // namespace detail

    template<typename... Args>
    inline void
    trace(Args&&... args)
    {
        detail::printLog(detail::getDefaultLogStream(), "TRACE", std::forward<Args>(args)...);
    }

    template<typename... Args>
    inline void
    log(Args&&... args)
    {
        detail::printLog(detail::getDefaultLogStream(), "LOG", std::forward<Args>(args)...);
    }

    template<typename... Args>
    inline void
    warn(Args&&... args)
    {
        detail::printLog(detail::getDefaultLogStream(), "WARN", std::forward<Args>(args)...);
    }

    template<typename... Args>
    inline void
    err(Args&&... args)
    {
        detail::printLog(detail::getErrorLogStream(), "ERROR", std::forward<Args>(args)...);
    }
} // namespace lemon::logger

#ifndef NDEBUG
#define LEMON_ASSERT(expr, ...)                                                                              \
    do {                                                                                                     \
        if (!(expr)) {                                                                                       \
            ::lemon::logger::detail::assertionError(#expr, __FILE__, __LINE__, __VA_ARGS__);                 \
            ::std::terminate();                                                                              \
        }                                                                                                    \
    } while (false)
#else
#define LEMON_ASSERT(expr)                                                                                   \
    do {                                                                                                     \
    } while (false)
#endif

#define LEMON_TODO() LEMON_ASSERT(false, "not implemented")
