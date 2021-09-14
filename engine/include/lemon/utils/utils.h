#pragma once

#include <iostream>
#include <mutex>
#include <optional>
#include <string>
#include <sstream>

namespace lemon::utils {
    namespace {
        static std::mutex printMut;
    }

    template<typename... Args>
    void
    print(Args&&... args) {
        std::stringstream stream;
        (stream << ... << args) << std::endl;
        std::cout << stream.str();
    }

    template<typename... Args>
    void
    printErr(Args&&... args) {
        std::stringstream stream;
        (stream << ... << args) << std::endl;
        std::cerr << stream.str();
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
