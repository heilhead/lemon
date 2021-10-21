#pragma once

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
