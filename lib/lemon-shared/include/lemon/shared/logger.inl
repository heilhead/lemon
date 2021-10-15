#pragma once

#include <ctime>
#include <cstdio>
#include <sstream>
#include <iomanip>
#include <lemon/shared/utils.h>

inline std::ostream&
lemon::logger::detail::Logger::getDefaultLogStream()
{
    return std::cout;
}

inline std::ostream&
lemon::logger::detail::Logger::getErrorLogStream()
{
    return std::cerr;
}

inline void
lemon::logger::detail::Logger::setLogLevel(LogLevel inLevel)
{
    level = inLevel;
}

inline lemon::logger::LogLevel
lemon::logger::detail::Logger::getLogLevel()
{
    return level;
}

template<typename... Args>
void
lemon::logger::detail::Logger::printLog(std::ostream& outStream, gsl::czstring<> category, Args&&... args)
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
lemon::logger::detail::Logger::assertionError(gsl::czstring<> expr, gsl::czstring<> file, int line,
                                              Args&&... args)
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
    ::lemon::logger::fatal(stream.view());
#else
    // For now, maintain _some_ compatibility with std++17, as required by `lemon-shader`.
    ::lemon::logger::fatal(stream.str());
#endif

    utils::terminate();
}

template<typename... Args>
void
lemon::logger::detail::Logger::unreachable(gsl::czstring<> file, int line, Args&&... args)
{
    if constexpr (sizeof...(Args) > 0) {
        assertionError(nullptr, file, line, std::forward<Args>(args)...);
    } else {
        assertionError(nullptr, file, line, "not implemented");
    }
}

template<typename... Args>
inline void
lemon::logger::trace(Args&&... args)
{
    auto& logger = lemon::logger::detail::Logger::get();
    if (logger.getLogLevel() >= LogLevel::Trace) {
        logger.printLog(logger.getDefaultLogStream(), "TRACE", std::forward<Args>(args)...);
    }
}

template<typename... Args>
inline void
lemon::logger::log(Args&&... args)
{
    auto& logger = lemon::logger::detail::Logger::get();
    if (logger.getLogLevel() >= LogLevel::Log) {
        logger.printLog(logger.getDefaultLogStream(), "LOG", std::forward<Args>(args)...);
    }
}

template<typename... Args>
inline void
lemon::logger::warn(Args&&... args)
{
    auto& logger = lemon::logger::detail::Logger::get();
    if (logger.getLogLevel() >= LogLevel::Warning) {
        logger.printLog(logger.getDefaultLogStream(), "WARN", std::forward<Args>(args)...);
    }
}

template<typename... Args>
inline void
lemon::logger::err(Args&&... args)
{
    auto& logger = lemon::logger::detail::Logger::get();
    if (logger.getLogLevel() >= LogLevel::Error) {
        logger.printLog(logger.getErrorLogStream(), "ERROR", std::forward<Args>(args)...);
    }
}

template<typename... Args>
inline void
lemon::logger::fatal(Args&&... args)
{
    auto& logger = lemon::logger::detail::Logger::get();
    if (logger.getLogLevel() >= LogLevel::Fatal) {
        logger.printLog(logger.getErrorLogStream(), "FATAL", std::forward<Args>(args)...);
    }
}
