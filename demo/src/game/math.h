#pragma once

namespace lemon::math {
    template<typename T>
    std::pair<T, T>
    sincos(T val);

    template<typename T>
    T
    square(T val);
} // namespace lemon::math

#include "math.inl"
