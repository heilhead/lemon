#pragma once

#include <lemon/game/actor/common.h>
#include <lemon/game/actor/GameObject.h>
#include <lemon/game/Transform.h>

namespace lemon::game {
    class Actor;
    class ActorComponent;
    class PositionableComponent;
    class RenderableComponent;

    template<class T>
    concept ActorComponentBase = Base<T, ActorComponent> && GameObjectBase<T>;

    template<class T>
    concept PositionableComponentBase = Base<T, PositionableComponent> && GameObjectBase<T>;

    template<class T>
    concept RenderableComponentBase = Base<T, RenderableComponent> && GameObjectBase<T>;

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

        template<Base<Actor> TActor = Actor>
        TActor*
        getOwner();

        template<Base<Actor> TActor = Actor>
        const TActor*
        getOwner() const;

        void
        addTickDependency(GameObject* pOtherComponent) override;

        void
        removeTickDependency(GameObject* pOtherComponent) override;

        PositionableComponent*
        getRoot();

        const PositionableComponent*
        getRoot() const;

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
        updateTickRecursive(GameObject* pParent, TickProxyHandle newHandle);

        ActorComponent*
        findTickingParent();
    };

    class PositionableComponent : public ActorComponent {
        struct TransformCache {
            glm::f32mat4 globalMatrix{kMatrixIdentity};
            glm::f32mat4 localMatrix{kMatrixIdentity};
            uint32_t generation{0};
            uint32_t parentGeneration{0};
            bool bDirty{true};
        };

        Transform transform{};
        mutable TransformCache transformCache{};

    public:
        PositionableComponent() = default;

        void
        onRegister() override;

        void
        onInitialize() override;

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

    protected:
        uint32_t
        updateGlobalTransform() const;

    private:
        void
        updateLocalTransform() const;
    };

    struct GameObjectRenderProxy {
        RenderableComponent* pRenderable;

        GameObjectRenderProxy(RenderableComponent* pRenderable);
    };

    class RenderableComponent : public PositionableComponent {
        RenderProxyHandle renderProxyHandle;

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

    template<Base<Actor> TActor>
    TActor*
    ActorComponent::getOwner()
    {
        return cast<TActor>(pOwner);
    }

    template<Base<Actor> TActor>
    const TActor*
    ActorComponent::getOwner() const
    {
        return cast<TActor>(pOwner);
    }
} // namespace lemon::game
