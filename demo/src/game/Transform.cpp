#include "Transform.h"

using namespace lemon::game;

Rotation::Rotation(const glm::quat& rot)
{
    const auto singTest = rot.z * rot.x - rot.w * rot.y;
    const auto yawY = 2.f * (rot.w * rot.z + rot.x * rot.y);
    const auto yawX = (1.f - 2.f * (math::square(rot.y) + math::square(rot.z)));

    constexpr auto singThreshold = 0.4999995f;

    if (singTest < -singThreshold) {
        pitch = -90.f;
        yaw = glm::degrees(std::atan2(yawY, yawX));
        roll = normalizeAxis(-yaw - 2.f * glm::degrees(std::atan2(rot.x, rot.w)));
    } else if (singTest > singThreshold) {
        pitch = 90.f;
        yaw = glm::degrees(std::atan2(yawY, yawX));
        roll = normalizeAxis(yaw - 2.f * glm::degrees(std::atan2(rot.x, rot.w)));
    } else {
        pitch = glm::degrees(std::asin(2.f * singTest));
        yaw = glm::degrees(std::atan2(yawY, yawX));
        roll = glm::degrees(std::atan2(-2.f * (rot.w * rot.x + rot.y * rot.z),
                                       (1.f - 2.f * (math::square(rot.x) + math::square(rot.y)))));
    }
}

Rotation::Rotation(const glm::f32vec3& euler)
{
    pitch = euler.y;
    yaw = euler.z;
    roll = euler.x;
}

glm::quat
Rotation::toQuat() const
{
    constexpr auto kHalfRads = glm::pi<float>() / 180.f / 2.f;

    auto [sp, cp] = math::sincos(std::fmod(pitch, 360.0f) * kHalfRads);
    auto [sy, cy] = math::sincos(std::fmod(yaw, 360.0f) * kHalfRads);
    auto [sr, cr] = math::sincos(std::fmod(roll, 360.0f) * kHalfRads);

    glm::quat qrot;
    qrot.x = cr * sp * sy - sr * cp * cy;
    qrot.y = -cr * sp * cy - sr * cp * sy;
    qrot.z = cr * cp * sy - sr * sp * cy;
    qrot.w = cr * cp * cy + sr * sp * sy;

    return qrot;
}

float
Rotation::clampAxis(float angle)
{
    angle = std::fmod(angle, 360.f);

    if (angle < 0.f) {
        angle += 360.f;
    }

    return angle;
}

float
Rotation::normalizeAxis(float angle)
{
    angle = clampAxis(angle);

    if (angle > 180.f) {
        angle -= 360.f;
    }

    return angle;
}
