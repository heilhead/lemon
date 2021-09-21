#pragma once

#include <iostream>
#include <mutex>
#include <optional>
#include <string>
#include <sstream>

namespace lemon::utils {
    namespace {
        static std::mutex printMut;

        inline std::ostream&
        getDefaultLogStream() {
            return std::cout;
        }

        inline std::ostream&
        getErrorLogStream() {
            return std::cerr;
        }
    } // namespace

    template<typename... Args>
    void
    log(Args&&... args) {
        std::stringstream stream;
        (stream << ... << args) << std::endl;
        getDefaultLogStream() << stream.str();
    }

    template<typename... Args>
    void
    logErr(Args&&... args) {
        std::stringstream stream;
        (stream << ... << args) << std::endl;
        getErrorLogStream() << stream.str();
    }

    void
    halt(const char* message, int code = 1);

    inline std::ostream&
    operator<<(std::ostream& os, std::optional<std::string> str) {
        if (str) {
            return os << *str;
        } else {
            return os << "[nullopt]";
        }
    }
} // namespace lemon::utils
