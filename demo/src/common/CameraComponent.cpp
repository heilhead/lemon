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
    GameWorld::get()->getCamera().update(getGlobalTransformMatrix());
}
