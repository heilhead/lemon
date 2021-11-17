#pragma once

inline bool
lemon::math::isPowerOfTwo(std::integral auto value)
{
    return value > 0 && ((value & (value - 1)) == 0);
}

template<std::integral T>
inline T
lemon::math::alignUp(T value, T alignment)
{
    LEMON_ASSERT(isPowerOfTwo(alignment));
    return (value + alignment - 1) & ~(alignment - 1);
}

template<std::integral T>
inline T
lemon::math::alignDown(T value, T alignment)
{
    LEMON_ASSERT(isPowerOfTwo(alignment));
    return value & ~(alignment - 1);
}

template<std::integral T>
inline bool
lemon::math::isAligned(T value, T alignment)
{
    LEMON_ASSERT(isPowerOfTwo(alignment));
    return (value & (alignment - 1)) == 0;
}

inline bool
lemon::math::isPtrAligned(const void* ptr, size_t alignment)
{
    LEMON_ASSERT(isPowerOfTwo(alignment));
    return isAligned(reinterpret_cast<size_t>(ptr), alignment);
}

template<typename T>
inline std::pair<T, T>
lemon::math::sincos(T val)
{
    return std::make_pair(std::sin(val), std::cos(val));
}

template<typename T>
inline T
lemon::math::square(T val)
{
    return val * val;
}

inline bool
lemon::math::isNearlyZero(std::floating_point auto val, float tolerance)
{
    return std::abs(val) <= tolerance;
}

template<size_t N, std::floating_point T, glm::qualifier Q>
bool
lemon::math::isNearlyZero(const glm::vec<N, T, Q>& v, float tolerance)
{
    static_assert(N <= 4, "only 2, 3 and 4-dimensional vectors are supported");

    bool bResult = isNearlyZero(v.x) && isNearlyZero(v.y);

    if constexpr (N >= 3) {
        bResult &= isNearlyZero(v.z);
    }

    if constexpr (N >= 4) {
        bResult &= isNearlyZero(v.w);
    }

    return bResult;
}

template<std::integral T>
inline constexpr T
lemon::math::fpack(float v)
{
    return static_cast<T>(std::numeric_limits<T>::max() * v);
}

template<typename T, size_t N, glm::qualifier Q>
inline constexpr glm::vec<N, T, Q>
lemon::math::fpack(const glm::vec<N, float, Q>& v)
{
    if constexpr (N == 1) {
        return glm::vec<N, T, Q>(fpack<T>(v.x));
    } else if constexpr (N == 2) {
        return glm::vec<N, T, Q>(fpack<T>(v.x), fpack<T>(v.y));
    } else if constexpr (N == 3) {
        return glm::vec<N, T, Q>(fpack<T>(v.x), fpack<T>(v.y), fpack<T>(v.z));
    } else if constexpr (N == 4) {
        return glm::vec<N, T, Q>(fpack<T>(v.x), fpack<T>(v.y), fpack<T>(v.z), fpack<T>(v.w));
    }
}
