#pragma once

#include "GameObject.h"

namespace lemon::game {
    class ActorComponent;

    template<class T>
    concept ActorComponentBase = Base<T, ActorComponent> && GameObjectBase<T>;

    class ActorComponent : public GameObject {
        friend class Actor;

    public:
        ActorComponent() : GameObject()
        {
            LEMON_TRACE_FN();

            tick.setTickType(GameObjectTickType::Component);
        }

        virtual ~ActorComponent();

    protected:
        virtual void
        onRegister()
        {
            LEMON_TRACE_FN();
        }

        virtual void
        onInitialize()
        {
            LEMON_TRACE_FN();
        }

        virtual void
        onUninitialize()
        {
            LEMON_TRACE_FN();
        }

        virtual void
        onUnregister()
        {
            LEMON_TRACE_FN();
        }
    };

    class PositionableComponent : public ActorComponent {
        struct TransformCache {
            const PositionableComponent* pParent{nullptr};
            glm::f32mat4 globalMatrix{kMatrixIdentity};
            glm::f32mat4 localMatrix{kMatrixIdentity};
            uint32_t generation{0};
            uint32_t parentGeneration{0};
            bool bDirty{true};
        };

        Transform transform{};
        mutable TransformCache transformCache{};

    public:
        void
        onRegister() override;

        glm::f32vec3
        getLocalPosition() const;

        glm::quat
        getLocalRotation() const;

        glm::f32vec3
        getLocalScale() const;

        void
        setLocalPosition(const glm::f32vec3& pos);

        void
        setLocalRotation(const glm::quat& rot);

        void
        setLocalScale(const glm::f32vec3& scale);

        const glm::f32mat4&
        getLocalTransformMatrix() const;

        const glm::f32mat4&
        getGlobalTransformMatrix() const;

    private:
        void
        updateLocalTransform() const;

        void
        updateGlobalTransform() const;
    };

    class RenderableComponent : public PositionableComponent {
        SlotMapHandle renderQueueHandle;

    public:
        void
        onStart() override;

        void
        onStop() override;
    };
} // namespace lemon::game
