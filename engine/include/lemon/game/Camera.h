#pragma once

#include <lemon/game/Transform.h>

namespace lemon::game {
    class Camera {
    public:
        enum class ProjectionType { Perspective, Ortho };

        struct UniformData {
            glm::f32mat4 matView{1.0};
            glm::f32mat4 matProjection{1.0};

            // Use `vec4` for padding purposes - structure size is expected to be multiple of 16.
            // Two additional `f32`s can be used to store i.e. fov values, if needed.
            glm::f32vec4 zData{1.0, 100.f, 0.f, 0.f};
        };

    private:
        Transform transform{};
        float width{1.f};
        float height{1.f};
        float zNear{1.f};
        float zFar{100.f};
        float fov{56.f};
        ProjectionType type{ProjectionType::Perspective};
        mutable UniformData uniformData;
        mutable bool bDirty{true};

    public:
        Camera() {}

        Transform&
        getTransform();

        const Transform&
        getTransform() const;

        void
        setProjectionType(ProjectionType inType);

        void
        setClipping(float inZNear, float inZFar);

        template<typename T>
        void
        setView(T inWidth, T inHeight);

        const UniformData&
        getUniformData() const;

    private:
        void
        updateUniformData() const;
    };
} // namespace lemon::game

inline lemon::game::Transform&
lemon::game::Camera::getTransform()
{
    return transform;
}

inline const lemon::game::Transform&
lemon::game::Camera::getTransform() const
{
    return transform;
}

inline void
lemon::game::Camera::setProjectionType(ProjectionType inType)
{
    type = inType;
    bDirty = true;
}

inline void
lemon::game::Camera::setClipping(float inZNear, float inZFar)
{
    zNear = inZNear;
    zFar = inZFar;
    bDirty = true;
}

template<typename T>
inline void
lemon::game::Camera::setView(T inWidth, T inHeight)
{
    width = static_cast<float>(inWidth);
    height = static_cast<float>(inHeight);
    bDirty = true;
}
