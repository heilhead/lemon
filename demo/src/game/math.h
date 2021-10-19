#pragma once

namespace lemon::math {
    template<std::floating_point T, glm::qualifier Q = glm::defaultp, size_t N = 3>
    glm::qua<T, Q>
    toOrientationQuat(const glm::vec<N, T, Q>& v);

    template<std::floating_point T, glm::qualifier Q = glm::defaultp>
    glm::qua<T, Q>
    toOrientationQuat(T x, T y, T z);

    template<typename T>
    std::pair<T, T>
    sincos(T val);

    template<typename T>
    T
    square(T val);
} // namespace lemon::math

#include "math.inl"
