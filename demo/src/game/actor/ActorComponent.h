#pragma once

#include "GameObject.h"

namespace lemon::game {
    class Actor;
    class ActorComponent;
    class RenderableComponent;

    template<class T>
    concept ActorComponentBase = Base<T, ActorComponent> && GameObjectBase<T>;

    class ActorComponent : public GameObject {
        friend class Actor;

        Actor* pOwner;

    public:
        ActorComponent();

        virtual ~ActorComponent();

        void
        enableTick(float interval = 0.f) override;

        void
        disableTick() override;

        Actor*
        getOwner();

        const Actor*
        getOwner() const;

    protected:
        virtual void
        onRegister();

        virtual void
        onInitialize();

        virtual void
        onUninitialize();

        virtual void
        onUnregister();

    private:
        void
        setOwner(Actor* pOwner);

        void
        attachTickRecursive(GameObject* pParent);

        void
        detachTickRecursive(GameObject* pParent);

        ActorComponent*
        findTickingParent();
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

    struct GameObjectRenderProxy {
        RenderableComponent* pRenderable;

        GameObjectRenderProxy(RenderableComponent* pRenderable);
    };

    class RenderableComponent : public PositionableComponent {
        GameObjectRenderProxyHandle renderProxyHandle;

    public:
        void
        onStart() override;

        void
        onStop() override;

    private:
        GameObjectRenderProxy
        createRenderProxy();

        void
        updateRenderProxy();
    };
} // namespace lemon::game
