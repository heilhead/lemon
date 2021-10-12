#pragma once

#include <iostream>
#include <optional>
#include <string>

namespace lemon::utils {
    void
    halt(const char* message, int code = 1);

    void
    terminate();

    inline std::ostream&
    operator<<(std::ostream& os, std::optional<std::string> str) {
        if (str) {
            return os << *str;
        } else {
            return os << "[nullopt]";
        }
    }
} // namespace lemon::utils
