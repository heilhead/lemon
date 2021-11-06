#include <lemon/game/actor/actors/FlyingCameraActor.h>
#include <lemon/game/actor/components/CameraComponent.h>
#include <lemon/game/actor/components/MovementComponent.h>

using namespace lemon;
using namespace lemon::game;
using namespace lemon::device;

FlyingCameraActor::FlyingCameraActor() : Actor()
{
    pCamera = addComponent<CameraComponent>();
    pMovement = addComponent<FlyingMovementComponent>();

    root = pCamera;
}

void
FlyingCameraActor::onStart()
{
    enableTick();

    hMouseClick =
        MouseListener::get()->getDelegate(MouseButton::Button2).add(&FlyingCameraActor::handleClick, this);
}

void
FlyingCameraActor::onStop()
{
    MouseListener::get()->getDelegate(MouseButton::Button2).remove(hMouseClick);
}

void
FlyingCameraActor::activateCamera() const
{
    pCamera->activateCamera();
}

void
FlyingCameraActor::deactivateCamera() const
{
    pCamera->deactivateCamera();
}

void
FlyingCameraActor::onTick(float dt)
{
    auto* pMouse = MouseListener::get();
    auto* pKeyboard = KeyboardListener::get();

    if (bCapturingMouse) {
        auto pos = pMouse->getPos();
        auto delta = pos - prevMousePos;

        if (!math::isNearlyZero(delta)) {
            pMovement->rotate(delta, 1.f);
        }

        prevMousePos = pos;
    }

    auto speedModifier = 1.f;

    if (pKeyboard->isKeyPressed(KeyCode::LeftShift)) {
        speedModifier *= 5.f;
    }

    if (pKeyboard->isKeyPressed(KeyCode::LeftAlt)) {
        speedModifier *= 0.2f;
    }

    auto movement = kVectorZero;

    if (pKeyboard->isKeyPressed(KeyCode::W)) {
        movement += kVectorForward;
    }

    if (pKeyboard->isKeyPressed(KeyCode::S)) {
        movement += kVectorBackward;
    }

    if (pKeyboard->isKeyPressed(KeyCode::A)) {
        movement += kVectorLeft;
    }

    if (pKeyboard->isKeyPressed(KeyCode::D)) {
        movement += kVectorRight;
    }

    if (!math::isNearlyZero(movement)) {
        pMovement->move(movement * speedModifier, dt, true);
    }

    movement = kVectorZero;

    if (pKeyboard->isKeyPressed(KeyCode::Space)) {
        movement += kVectorUp;
    }

    if (pKeyboard->isKeyPressed(KeyCode::LeftControl)) {
        movement += kVectorDown;
    }

    if (!math::isNearlyZero(movement)) {
        pMovement->move(movement * speedModifier, dt, true);
    }
}

void
FlyingCameraActor::handleClick(KeyEvent evt, KeyMod mods)
{
    if (evt == KeyEvent::Press) {
        MouseListener::get()->setCursorMode(CursorMode::Raw);
        bCapturingMouse = true;
        prevMousePos = MouseListener::get()->getPos();
    } else if (evt == KeyEvent::Release) {
        MouseListener::get()->setCursorMode(CursorMode::Normal);
        bCapturingMouse = false;
    }
}
