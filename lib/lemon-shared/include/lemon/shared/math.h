#pragma once

#include <lemon/shared/logger.h>
#include <lemon/pch/glm.h>
#include <cstdint>
#include <concepts>

namespace lemon::math {
    constexpr auto kBigNumber = 3.4e+38f;
    constexpr auto kSmallNumber = 1.e-8f;
    constexpr auto kKindaSmallNumber = 1.e-4f;
    constexpr auto kPI = 3.1415926535897932f;
    constexpr auto kInvPI = 0.31830988618f;
    constexpr auto kHalfPI = 1.57079632679f;

    bool
    isPowerOfTwo(std::integral auto value);

    template<std::integral T>
    T
    alignUp(T value, T alignment);

    template<std::integral T>
    T
    alignDown(T value, T alignment);

    template<std::integral T>
    bool
    isAligned(T value, T alignment);

    bool
    isPtrAligned(const void* ptr, size_t alignment);

    template<typename T>
    std::pair<T, T>
    sincos(T val);

    template<typename T>
    T
    square(T val);

    bool
    isNearlyZero(std::floating_point auto val, float tolerance = kSmallNumber);

    template<size_t N, std::floating_point T, glm::qualifier Q>
    bool
    isNearlyZero(const glm::vec<N, T, Q>& v, float tolerance = kSmallNumber);

    template<std::integral T>
    constexpr T
    fpack(float v);

    template<typename T, size_t N, glm::qualifier Q = glm::packed_highp>
    constexpr glm::vec<N, T, Q>
    fpack(const glm::vec<N, float, Q>& v);
} // namespace lemon::math

#include <lemon/shared/math.inl>
