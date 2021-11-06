#pragma once

#include <lemon/game/Camera.h>
#include <lemon/game/actor/ActorComponent.h>

namespace lemon::game {
    class CameraComponent : public PositionableComponent {
        friend class GameWorld;

        Camera camera;

    public:
        void
        activateCamera();

        void
        deactivateCamera();

        Camera&
        getCamera();

        const Camera&
        getCamera() const;

    private:
        void
        updateWorldCamera(Camera& worldCamera);
    };
} // namespace lemon::game
