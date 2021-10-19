#pragma once

template<std::floating_point T, glm::qualifier Q, size_t N>
inline glm::qua<T, Q>
lemon::math::toOrientationQuat(const glm::vec<N, T, Q>& v)
{
    return toOrientationQuat<T, Q>(v.x, v.y, v.z);
}

template<std::floating_point T, glm::qualifier Q>
glm::qua<T, Q>
lemon::math::toOrientationQuat(T x, T y, T z)
{
    const float yaw = std::atan2(y, x);
    const float pitch = std::atan2(z, std::sqrt(x * x + y * y));

    const auto [sp, cp] = sincos(pitch * 0.5f);
    const auto [sy, cy] = sincos(yaw * 0.5f);

    glm::qua<T, Q> q;

    q.x = sp * sy;
    q.y = -sp * cy;
    q.z = cp * sy;
    q.w = cp * cy;

    return q;
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
