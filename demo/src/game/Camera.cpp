#include "Camera.h"

using namespace lemon::game;

const Camera::UniformData&
Camera::getUniformData() const
{
    updateUniformData();
    return uniformData;
}

inline void
Camera::updateUniformData() const
{
    const auto rot = transform.getRotation();
    const auto fwd = kVectorForward * rot;
    const auto up = kVectorUp * rot;

    uniformData.matView = glm::lookAt(transform.getPosition(), fwd, up);
    uniformData.zData.x = zNear;
    uniformData.zData.y = zFar;

    if (bDirty) {
        if (type == ProjectionType::Perspective) {
            uniformData.matProjection = glm::perspective(glm::radians(fov), width / height, zNear, zFar);
        } else {
            // uniformData.matProjection = glm::ortho(0.f, 0.f, 0.f, 0.f, zNear, zFar);
            LEMON_TODO("ortho camera is not implemented");
        }

        bDirty = false;
    }
}
