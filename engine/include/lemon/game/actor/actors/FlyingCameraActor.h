#pragma once

#include <lemon/game/actor/Actor.h>
#include <lemon/misc/Delegate.h>
#include <lemon/device/Device.h>

namespace lemon::game {
    class FlyingCameraActor : public Actor {
        class CameraComponent* pCamera;
        class FlyingMovementComponent* pMovement;

        glm::f64vec2 prevMousePos{0.f, 0.f};
        DelegateHandle hMouseClick;
        bool bCapturingMouse{false};

    public:
        FlyingCameraActor();

        void
        onTick(float dt) override;

        void
        onStart() override;

        void
        onStop() override;

        void
        activateCamera() const;

        void
        deactivateCamera() const;

    private:
        void
        handleClick(device::KeyEvent evt, device::KeyMod mods);
    };
} // namespace lemon::game
