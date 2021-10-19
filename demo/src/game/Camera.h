#pragma once

#include "Transform.h"

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
        float fov{90.f};

        ProjectionType type{ProjectionType::Perspective};
        mutable UniformData uniformData;

        mutable bool bDirty{false};

    public:
        Camera() {}

        inline Transform&
        getTransform()
        {
            return transform;
        }

        inline const Transform&
        getTransform() const
        {
            return transform;
        }

        inline void
        setProjectionType(ProjectionType inType)
        {
            type = inType;
            bDirty = true;
        }

        inline void
        setClipping(float inZNear, float inZFar)
        {
            zNear = inZNear;
            zFar = inZFar;
            bDirty = true;
        }

        template<typename T>
        inline void
        setView(T inWidth, T inHeight)
        {
            width = static_cast<float>(inWidth);
            height = static_cast<float>(inHeight);
            bDirty = true;
        }

        inline const UniformData&
        getUniformData() const
        {
            updateUniformData();
            return uniformData;
        }

        inline UniformData&
        getUniformData()
        {
            updateUniformData();
            return uniformData;
        }

    private:
        inline void
        updateUniformData() const
        {
            uniformData.matView = transform.getMatrix();
            uniformData.zData.x = zNear;
            uniformData.zData.y = zFar;

            if (bDirty) {
                if (type == ProjectionType::Perspective) {
                    uniformData.matProjection =
                        glm::perspective(glm::radians(fov), width / height, zNear, zFar);
                } else {
                    // uniformData.matProjection = glm::ortho(0.f, 0.f, 0.f, 0.f, zNear, zFar);
                    LEMON_TODO("ortho camera is not implemented");
                }

                bDirty = false;
            }
        }
    };
} // namespace lemon::game
