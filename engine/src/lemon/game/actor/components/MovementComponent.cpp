#include <lemon/game/actor/components/MovementComponent.h>

using namespace lemon;
using namespace lemon::game;

void
FlyingMovementComponent::move(const glm::f32vec3& axis, float factor, bool bRelative)
{
    auto abs = axis * factor * movementSpeed * globalSpeed;

    if (bRelative) {
        abs = abs * rotation.toQuat();
    }

    auto* root = getRoot();
    root->setLocalPosition(root->getLocalPosition() + abs);
}

void
FlyingMovementComponent::rotate(const glm::f64vec2& delta, float factor)
{
    auto deltaYaw = static_cast<float>(delta.x) * factor * horizontalSensitivity * globalSensitivity;
    auto deltaPitch = -static_cast<float>(delta.y) * factor * verticalSensitivity * globalSensitivity;

    rotation.pitch = std::clamp(rotation.pitch + deltaPitch, -90.f, 90.f);
    rotation.yaw = Rotation::clampAxis(rotation.yaw + deltaYaw);

    getRoot()->setLocalRotation(rotation.toQuatYawPitch());
}

void
FlyingMovementComponent::setSpeed(float factor)
{
    globalSpeed = factor;
}

void
FlyingMovementComponent::setSensitivity(float factor)
{
    globalSensitivity = factor;
}
