#pragma once

namespace lemon::game {
    class CameraControllerBase : NonCopyable {
        Camera camera;

    public:
        CameraControllerBase() = default;
        virtual ~CameraControllerBase() = default;

        virtual void
        handlePositionInput(const glm::f32vec3& axis, float factor, bool bRelative)
        {
        }

        virtual void
        handleRotationInput(const glm::f64vec2& delta, float factor)
        {
        }

        const Camera&
        getCamera() const;

        Camera&
        getCamera();
    };

    class FirstPersonCameraController : public CameraControllerBase {
        Rotation rotation;

    public:
        float horizontalSensitivity = 0.038f;
        float verticalSensitivity = 0.025f;
        float movementSpeed = 350.f;

        void
        handlePositionInput(const glm::f32vec3& axis, float factor, bool bRelative) override;

        void
        handleRotationInput(const glm::f64vec2& delta, float factor) override;
    };
} // namespace lemon::game

inline const lemon::game::Camera&
lemon::game::CameraControllerBase::getCamera() const
{
    return camera;
}

inline lemon::game::Camera&
lemon::game::CameraControllerBase::getCamera()
{
    return camera;
}
