#pragma once

#include <ctime>
#include <cstdio>
#include <sstream>
#include <iostream>
#include <iomanip>

namespace lemon::logger {
    namespace {
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
    } // namespace

    template<typename... Args>
    inline void
    trace(Args&&... args)
    {
        printLog(getDefaultLogStream(), "TRACE", std::forward<Args>(args)...);
    }

    template<typename... Args>
    inline void
    log(Args&&... args)
    {
        printLog(getDefaultLogStream(), "LOG", std::forward<Args>(args)...);
    }

    template<typename... Args>
    inline void
    warn(Args&&... args)
    {
        printLog(getDefaultLogStream(), "WARN", std::forward<Args>(args)...);
    }

    template<typename... Args>
    inline void
    err(Args&&... args)
    {
        printLog(getErrorLogStream(), "ERROR", std::forward<Args>(args)...);
    }
} // namespace lemon::logger