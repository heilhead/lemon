#pragma once

#include <ctime>
#include <cstdio>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <lemon/shared/utils.h>

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

#if __cplusplus >= 202002L
            outStream << stream.view();
#else
            outStream << stream.str();
#endif
        }

        template<typename... Args>
        void
        assertionError(const char* expr, const char* file, int line, Args&&... args)
        {
            std::stringstream stream;
            stream << "Assertion ";

            if (expr != nullptr) {
                stream << "`" << expr << "` ";
            }

            stream << "failed in " << file << " line " << line;

            if constexpr (sizeof...(Args) > 0) {
                stream << ": ";
                (stream << ... << args);
            }

#if __cplusplus >= 202002L
            printLog(getErrorLogStream(), "FATAL", stream.view());
#else
            // For now, maintain _some_ compatibility with std++17, as required by `lemon-shader`.
            printLog(getErrorLogStream(), "FATAL", stream.str());
#endif

            utils::terminate();
        }

        template<typename... Args>
        void
        unreachable(const char* file, int line, Args&&... args)
        {
            if constexpr (sizeof...(Args) > 0) {
                assertionError(nullptr, file, line, std::forward<Args>(args)...);
            } else {
                assertionError(nullptr, file, line, "not implemented");
            }
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

#ifdef NDEBUG
#define LEMON_ASSERT(expression) ((void)0)
#else
#define LEMON_ASSERT(expression, ...)                                                                        \
    (void)((!!(expression)) ||                                                                               \
           (::lemon::logger::detail::assertionError(#expression, __FILE__, __LINE__, __VA_ARGS__), 0))
#endif

#define LEMON_TODO(...) ::lemon::logger::detail::unreachable(__FILE__, __LINE__, __VA_ARGS__)
