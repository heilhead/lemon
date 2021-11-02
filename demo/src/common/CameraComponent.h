#pragma once

#include <lemon/game.h>
#include <lemon/game/actor/ActorComponent.h>

namespace lemon::game {
    class CameraComponent : public PositionableComponent {
    public:
        void
        onStart() override;

        void
        onTick(float dt) override;
    };
} // namespace lemon::game
