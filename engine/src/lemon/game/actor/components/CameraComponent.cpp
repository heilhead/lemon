#include <lemon/game/actor/components/CameraComponent.h>
#include <lemon/game/actor/GameWorld.h>

using namespace lemon;
using namespace lemon::game;

void
CameraComponent::activateCamera()
{
    GameWorld::get()->setCameraActive(this);
}

void
CameraComponent::deactivateCamera()
{
    GameWorld::get()->setCameraInactive(this);
}

Camera&
CameraComponent::getCamera()
{
    return camera;
}

const Camera&
CameraComponent::getCamera() const
{
    return camera;
}

void
CameraComponent::updateWorldCamera(Camera& worldCamera)
{
    if (camera.isDirty()) {
        camera.update(getGlobalTransformMatrix());
        worldCamera = camera;
    } else {
        worldCamera.update(getGlobalTransformMatrix());
    }
}
