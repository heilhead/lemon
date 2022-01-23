#pragma once

#include <iostream>
#include <optional>
#include <string>
#include <gsl/gsl>

namespace lemon::utils {
    void
    halt(gsl::czstring<> message, int code = 1);

    void
    terminate();

    inline std::ostream&
    operator<<(std::ostream& os, std::optional<std::string> str)
    {
        if (str) {
            return os << *str;
        } else {
            return os << "[nullopt]";
        }
    }

    std::string
    ws2s(const std::wstring& str);

    std::wstring
    s2ws(const std::string& str);

    template<class T>
    inline uint64_t
    getTypeID()
    {
        return typeid(T).hash_code();
    }
} // namespace lemon::utils
