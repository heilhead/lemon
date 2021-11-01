#pragma once

#include <lemon/game/actor/Actor.h>

namespace lemon::game {
    class FlyingCameraActor : public Actor {
        class CameraComponent* pCamera;
        class MovementComponent* pMovement;

    public:
        FlyingCameraActor();
    };
} // namespace lemon::game
