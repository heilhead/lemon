#pragma once

#include <lemon/game/common.h>

namespace lemon::game {
    class Rotation {
    public:
        float pitch;
        float yaw;
        float roll;

        Rotation();

        explicit Rotation(float val);

        explicit Rotation(const glm::quat& rot);

        explicit Rotation(const glm::f32vec3& euler);

        Rotation(float pitch, float yaw, float roll);

        glm::quat
        toQuat() const;

        glm::quat
        toQuatYawPitch() const;

        void
        fromQuat(const glm::quat& rot);

        glm::f32vec3
        toEuler() const;

        void
        fromEuler(const glm::f32vec3& euler);

        static float
        clampAxis(float angle);

        static float
        normalizeAxis(float angle);
    };

    class Transform {
        glm::f32vec3 position = kVectorZero;
        glm::quat rotation = kQuatIdentity;
        glm::f32vec3 scale = kVectorOne;

    public:
        Transform() {}

        void
        setPosition(const glm::f32vec3& pos);

        void
        setPosition(float x, float y, float z);

        void
        setRotation(const glm::f32vec3& euler);

        void
        setRotation(float pitch, float yaw, float roll);

        void
        setRotation(const glm::quat& rot);

        void
        setScale(const glm::f32vec3& scale);

        void
        setScale(float x, float y, float z);

        void
        setScale(float s);

        void
        lookAt(const glm::f32vec3& pos);

        void
        lookAt(float x, float y, float z);

        void
        translate(const glm::f32vec3& pos);

        void
        translate(float x, float y, float z);

        glm::f32vec3
        getPosition() const;

        glm::quat
        getRotation() const;

        glm::f32mat4
        getMatrix() const;

        glm::f32mat4
        getMatrixNoScaling() const;
    };
} // namespace lemon::game

inline glm::quat
lemon::game::Rotation::toQuat() const
{
    // N.B. GLM's euler-to-quat implementation is different in that the transformations are applied all
    // together as a matrix, which results in a bit unintuitive orientation.

    auto qYaw = glm::rotate(kQuatIdentity, glm::radians(yaw), kVectorYAxis);
    auto qPitch = glm::rotate(kQuatIdentity, glm::radians(pitch), kVectorXAxis * qYaw);
    auto qRoll = glm::rotate(kQuatIdentity, glm::radians(roll), kVectorZAxis * qYaw * qPitch);

    return qYaw * qPitch * qRoll;
}

inline glm::quat
lemon::game::Rotation::toQuatYawPitch() const
{
    auto qYaw = glm::rotate(kQuatIdentity, glm::radians(yaw), kVectorYAxis);
    auto qPitch = glm::rotate(kQuatIdentity, glm::radians(pitch), kVectorXAxis * qYaw);

    return qYaw * qPitch;
}

inline void
lemon::game::Rotation::fromQuat(const glm::quat& rot)
{
    fromEuler(glm::degrees(glm::eulerAngles(rot)));
}

inline glm::f32vec3
lemon::game::Rotation::toEuler() const
{
    return glm::f32vec3(pitch, yaw, roll);
}

inline void
lemon::game::Rotation::fromEuler(const glm::f32vec3& euler)
{
    pitch = euler.x;
    yaw = euler.y;
    roll = euler.z;
}

inline void
lemon::game::Transform::setPosition(const glm::f32vec3& inPos)
{
    position = inPos;
}

inline void
lemon::game::Transform::setPosition(float x, float y, float z)
{
    position.x = x;
    position.y = y;
    position.z = z;
}

inline void
lemon::game::Transform::setRotation(const glm::f32vec3& euler)
{
    rotation = Rotation(euler).toQuat();
}

inline void
lemon::game::Transform::setRotation(float pitch, float yaw, float roll)
{
    rotation = Rotation(pitch, yaw, roll).toQuat();
}

inline void
lemon::game::Transform::setRotation(const glm::quat& rot)
{
    rotation = rot;
}

inline void
lemon::game::Transform::setScale(const glm::f32vec3& inScale)
{
    scale = inScale;
}

inline void
lemon::game::Transform::setScale(float x, float y, float z)
{
    scale.x = x;
    scale.y = y;
    scale.z = z;
}

inline void
lemon::game::Transform::setScale(float s)
{
    scale.x = s;
    scale.y = s;
    scale.z = s;
}

inline void
lemon::game::Transform::lookAt(const glm::f32vec3& pos)
{
    lookAt(pos.x, pos.y, pos.z);
}

inline void
lemon::game::Transform::lookAt(float x, float y, float z)
{
    auto dir = glm::normalize(position - glm::f32vec3(x, y, z));
    setRotation(glm::quatLookAt(dir, kVectorUp));
}

inline void
lemon::game::Transform::translate(const glm::f32vec3& inPos)
{
    position += inPos;
}

inline void
lemon::game::Transform::translate(float x, float y, float z)
{
    position += glm::f32vec3(x, y, z);
}

inline glm::f32vec3
lemon::game::Transform::getPosition() const
{
    return position;
}

inline glm::quat
lemon::game::Transform::getRotation() const
{
    return rotation;
}
