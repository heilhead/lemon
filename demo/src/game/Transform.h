#pragma once

namespace lemon::game {
    class Transform {
        glm::f32vec3 position{0.f, 0.f, 0.f};
        glm::f32vec3 rotation{0.f, 0.f, 0.f};
        glm::f32vec3 scale{1.f, 1.f, 1.f};

    public:
        Transform() {}

        inline void
        setPosition(const glm::f32vec3& inPos)
        {
            position = inPos;
        }

        inline void
        setPosition(float x, float y, float z)
        {
            position.x = x;
            position.y = y;
            position.z = z;
        }

        inline void
        setRotation(const glm::f32vec3& inRot)
        {
            rotation = inRot;
        }

        inline void
        setRotation(float x, float y, float z)
        {
            rotation.x = x;
            rotation.y = y;
            rotation.z = z;
        }

        inline void
        setScale(const glm::f32vec3& inScale)
        {
            scale = inScale;
        }

        inline void
        setScale(float x, float y, float z)
        {
            scale.x = x;
            scale.y = y;
            scale.z = z;
        }

        inline void
        setScale(float s)
        {
            scale.x = s;
            scale.y = s;
            scale.z = s;
        }

        inline void
        translate(const glm::f32vec3& inPos)
        {
            position += inPos;
        }

        inline void
        translate(float x, float y, float z)
        {
            position += glm::f32vec3(x, y, z);
        }

        inline void
        rotate(const glm::f32vec3& inRot)
        {
            rotation += inRot;
        }

        inline void
        rotate(float x, float y, float z)
        {
            rotation += glm::f32vec3(x, y, z);
        }

        inline glm::f32vec3
        getPosition() const
        {
            return position;
        }

        inline glm::f32vec3
        getRotation() const
        {
            return rotation;
        }

        glm::f32mat4
        getMatrix() const
        {
            // glm::f32vec3 baseRot(90.f, 0.f, 0.f);
            // auto pos = glm::quat(glm::radians(baseRot)) * position;

            glm::quat rot(glm::radians(rotation));

            auto fwd = rot * glm::f32vec3(0.f, 1.f, 0.f);
            auto up = rot * glm::f32vec3(0.f, 0.f, 1.f);

            return glm::rotate(glm::scale(glm::lookAt(position, position + fwd, up), scale),
                               glm::radians(90.f), glm::f32vec3(1.f, 0.f, 0.f));
        }
    };
} // namespace lemon::game
