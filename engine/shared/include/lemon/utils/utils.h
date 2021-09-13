#pragma once

#include <iostream>
#include <mutex>
#include <optional>
#include <string>

namespace lemon::utils {
    namespace {
        std::mutex printMut;
    }

    template<typename... Args>
    void
    tprint(Args&&... args) {
        std::lock_guard lg(printMut);
        (std::cout << ... << args) << std::endl;
    }

    template<typename... Args>
    void
    print(Args&&... args) {
        (std::cout << ... << args) << std::endl;
    }

    template<typename... Args>
    void
    printErr(Args&&... args) {
        (std::cerr << ... << args) << std::endl;
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
