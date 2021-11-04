#include "CameraComponent.h"

using namespace lemon;
using namespace lemon::game;

void
CameraComponent::onStart()
{
    enableTick();
}

void
CameraComponent::onTick(float dt)
{
    // TODO: This should not be here. Figure out a better way of exposing and controlling camera.
    GameWorld::get()->getCamera().update(getGlobalTransformMatrix());
}
