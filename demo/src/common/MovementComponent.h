#pragma once

#include <lemon/game/actor/ActorComponent.h>

namespace lemon::game {
    class MovementComponent : public ActorComponent {
        virtual void
        move(const glm::f32vec3& axis, float factor, bool bRelative)
        {
        }

        virtual void
        rotate(const glm::f64vec2& delta, float factor)
        {
        }
    };

    class FlyingMovementComponent : public MovementComponent {
        Rotation rotation;

    public:
        float horizontalSensitivity = 0.038f;
        float verticalSensitivity = 0.025f;
        float movementSpeed = 350.f;

        void
        move(const glm::f32vec3& axis, float factor, bool bRelative) override;

        void
        rotate(const glm::f64vec2& delta, float factor) override;
    };
} // namespace lemon::game
