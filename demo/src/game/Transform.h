#pragma once

#include "math.h"
#include "common.h"

namespace lemon::game {
    class Rotation {
        float pitch;
        float yaw;
        float roll;

    public:
        Rotation() : pitch{0.f}, yaw{0.f}, roll{0.f} {}
        explicit Rotation(float val) : pitch{val}, yaw{val}, roll{val} {}
        explicit Rotation(const glm::quat& rot);
        explicit Rotation(const glm::f32vec3& euler);
        Rotation(float pitch, float yaw, float roll) : pitch{pitch}, yaw{yaw}, roll{roll} {}

        glm::quat
        toQuat() const;

        inline glm::f32vec3
        toEuler() const
        {
            return glm::f32vec3(roll, pitch, yaw);
        }

        static float
        clampAxis(float angle);

        static float
        normalizeAxis(float angle);
    };

    class Transform {
        glm::f32vec3 position{0.f, 0.f, 0.f};
        glm::quat rotation{};
        glm::f32vec3 scale{1.f, 1.f, 1.f};

    public:
        static constexpr auto forward = glm::f32vec3{1.f, 0.f, 0.f};
        static constexpr auto right = glm::f32vec3{0.f, 1.f, 0.f};
        static constexpr auto up = glm::f32vec3{0.f, 0.f, 1.f};
        static constexpr auto identity = glm::f32mat4{1.0};

        Transform() {}

        inline void
        setPosition(const glm::f32vec3& inPos)
        {
            position = inPos;
        }

        inline void
        setPosition(float x, float y, float z)
        {
            position.x = x;
            position.y = y;
            position.z = z;
        }

        inline void
        setRotation(const glm::f32vec3& euler)
        {
            rotation = Rotation(euler).toQuat();
        }

        inline void
        setRotation(float pitch, float yaw, float roll)
        {
            rotation = Rotation(pitch, yaw, roll).toQuat();
        }

        inline void
        setRotation(const glm::quat& rot)
        {
            rotation = rot;
        }

        inline void
        setScale(const glm::f32vec3& inScale)
        {
            scale = inScale;
        }

        inline void
        setScale(float x, float y, float z)
        {
            scale.x = x;
            scale.y = y;
            scale.z = z;
        }

        inline void
        setScale(float s)
        {
            scale.x = s;
            scale.y = s;
            scale.z = s;
        }

        void
        lookAt(float x, float y, float z)
        {
            auto dir = glm::normalize(position - glm::f32vec3(x, y, z));
            auto q1 = math::toOrientationQuat(dir);
            auto q2 = math::toOrientationQuat(right);
            auto diff = q2 * glm::inverse(q1);

            setRotation(diff);
        }

        inline void
        translate(const glm::f32vec3& inPos)
        {
            position += inPos;
        }

        inline void
        translate(float x, float y, float z)
        {
            position += glm::f32vec3(x, y, z);
        }

        inline glm::f32vec3
        getPosition() const
        {
            return position;
        }

        inline glm::quat
        getRotation() const
        {
            return rotation;
        }

        glm::f32mat4
        getMatrix() const
        {
            auto t = glm::translate(identity, position);
            auto r = glm::mat4_cast(rotation);
            auto s = glm::scale(identity, scale);
            return t * r * s;
        }
    };
} // namespace lemon::game
