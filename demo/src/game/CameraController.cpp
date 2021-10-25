#include "CameraController.h"

using namespace lemon;
using namespace lemon::game;

void
FirstPersonCameraController::handlePositionInput(const glm::f32vec3& axis, float factor, bool bRelative)
{
    auto abs = axis * factor * movementSpeed;

    if (bRelative) {
        abs = abs * rotation.toQuat();
    }

    getCamera().getTransform().translate(abs);
}

void
FirstPersonCameraController::handleRotationInput(const glm::f64vec2& delta, float factor)
{
    auto deltaYaw = static_cast<float>(delta.x) * factor * horizontalSensitivity;
    auto deltaPitch = -static_cast<float>(delta.y) * factor * verticalSensitivity;

    rotation.pitch = std::clamp(rotation.pitch + deltaPitch, -90.f, 90.f);
    rotation.yaw = Rotation::clampAxis(rotation.yaw + deltaYaw);

    getCamera().getTransform().setRotation(rotation.toQuatYawPitch());
}
