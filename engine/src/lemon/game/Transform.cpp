#include <lemon/game/Transform.h>

using namespace lemon::game;

Rotation::Rotation() : Rotation(0.f, 0.f, 0.f) {}

Rotation::Rotation(float val) : Rotation(val, val, val) {}

Rotation::Rotation(float pitch, float yaw, float roll) : pitch{pitch}, yaw{yaw}, roll{roll} {}

Rotation::Rotation(const glm::quat& rot)
{
    fromQuat(rot);
}

Rotation::Rotation(const glm::f32vec3& euler)
{
    fromEuler(euler);
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

glm::f32mat4
Transform::getMatrix() const
{
    auto t = glm::translate(kMatrixIdentity, position);
    auto r = glm::mat4_cast(rotation);
    auto s = glm::scale(kMatrixIdentity, scale);
    return t * r * s;
}

glm::f32mat4
Transform::getMatrixNoScaling() const
{
    auto t = glm::translate(kMatrixIdentity, position);
    auto r = glm::mat4_cast(rotation);
    return t * r;
}
