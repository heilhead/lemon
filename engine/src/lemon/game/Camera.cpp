#include <lemon/game/Camera.h>
#include <lemon/device/Device.h>

using namespace lemon::game;

Camera::Camera()
{
    if (auto* pDevice = device::Device::get()) {
        auto [width, height] = pDevice->getWindow()->getSize();
        setView(width, height);
    }
}

const Camera::UniformData&
Camera::getUniformData() const
{
    return uniformData;
}

void
Camera::update()
{
    updateView(transform);
    updateProjection();
}

void
Camera::update(const Transform& t)
{
    updateView(t);
    updateProjection();
}

void
Camera::update(const glm::f32mat4& matWorld)
{
    updateView(matWorld);
    updateProjection();
}

inline void
Camera::updateView(const glm::f32mat4& matWorld)
{
    const auto pos = glm::f32vec3(matWorld * glm::f32vec4(kVectorZero, 1.f));
    const auto fwd = glm::f32vec3(glm::f32vec4(kVectorForward, 1.f) * matWorld);
    const auto up = glm::f32vec3(glm::f32vec4(kVectorUp, 1.f) * matWorld);

    uniformData.matView = glm::lookAt(pos, pos + fwd, up);
}

inline void
Camera::updateView(const Transform& t)
{
    const auto pos = t.getPosition();
    const auto rot = t.getRotation();
    const auto fwd = kVectorForward * rot;
    const auto up = kVectorUp * rot;

    uniformData.matView = glm::lookAt(pos, pos + fwd, up);
}

inline void
Camera::updateProjection()
{
    if (bDirty) {
        uniformData.zData.x = zNear;
        uniformData.zData.y = zFar;

        if (type == ProjectionType::Perspective) {
            uniformData.matProjection = glm::perspective(glm::radians(fov), width / height, zNear, zFar);
        } else {
            // uniformData.matProjection = glm::ortho(0.f, 0.f, 0.f, 0.f, zNear, zFar);
            LEMON_TODO("ortho camera is not implemented");
        }

        bDirty = false;
    }
}
