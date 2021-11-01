#include "FlyingCameraActor.h"
#include "CameraComponent.h"
#include "MovementComponent.h"

using namespace lemon;
using namespace lemon::game;

FlyingCameraActor::FlyingCameraActor() : Actor()
{
    pCamera = addComponent<CameraComponent>();
    pMovement = addComponent<MovementComponent>();

    root = pCamera;
}
