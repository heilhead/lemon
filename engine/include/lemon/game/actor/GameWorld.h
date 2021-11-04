#pragma once

#include <lemon/game/actor/ActorComponent.h>
#include <lemon/game/actor/Actor.h>
#include <lemon/game/actor/GameObjectStore.h>
#include <lemon/game/actor/TickGroup.h>
#include <lemon/game/Camera.h>

namespace lemon::game {
    class GameWorld : public UnsafeSingleton<GameWorld> {
        GameObjectStore store;

        SlotMap<Actor*, GameObjectWorldHandle> actors;
        TickGroup tickingActors;
        TickGroup tickingComponents;
        SlotMap<GameObjectRenderProxy, RenderProxyHandle> renderableComponents;
        Camera camera;

        double lastUpdateTime;

    public:
        GameWorld();

        template<ActorBase TActor>
        GameObjectHandle<TActor>
        createActor(const glm::f32vec3& pos = kVectorZero, const glm::quat& rot = kQuatIdentity,
                    const glm::f32vec3& scale = kVectorOne);

        template<ActorBase TActor>
        void
        destroyActor(GameObjectHandle<TActor> handle);

        template<ActorBase TActor>
        GameObjectHandle<TActor>
        addActor(TActor* pActor, const glm::f32vec3& pos = kVectorZero, const glm::quat& rot = kQuatIdentity,
                 const glm::f32vec3& scale = kVectorOne);

        void
        removeActor(Actor* pActor);

        GameObjectRenderProxy*
        getRenderProxy(RenderProxyHandle handle);

        static GameObjectStore*
        getStoreInternal();

        void
        updateInternal(double time);

        GameObjectWorldHandle
        registerActorInternal(Actor* pActor);

        void
        unregisterActorInternal(GameObjectWorldHandle handle);

        RenderProxyHandle
        registerRenderableComponentInternal(const GameObjectRenderProxy& proxy);

        void
        unregisterRenderableComponentInternal(RenderProxyHandle handle);

        TickGroup*
        getActorTickGroup();

        TickGroup*
        getComponentTickGroup();

        const Camera&
        getCamera() const;

        Camera&
        getCamera();

        double
        getTime() const
        {
            return lastUpdateTime;
        }

    private:
        void
        tick(TickProxy& proxy, double time, float dt);
    };

    template<ActorBase TActor>
    GameObjectHandle<TActor>
    GameWorld::createActor(const glm::f32vec3& pos, const glm::quat& rot, const glm::f32vec3& scale)
    {
        auto* pActor = store.create<TActor>();
        pActor->seal();

        return addActor(pActor, pos, rot, scale);
    }

    template<ActorBase TActor>
    void
    GameWorld::destroyActor(GameObjectHandle<TActor> handle)
    {
        auto* pActor = handle.get();

        if (pActor->bAddedToWorld) {
            removeActor(pActor);
        }

        store.destroy(pActor);
    }

    template<ActorBase TActor>
    GameObjectHandle<TActor>
    GameWorld::addActor(TActor* pActor, const glm::f32vec3& pos, const glm::quat& rot,
                        const glm::f32vec3& scale)
    {
        if (pActor->bAddedToWorld) {
            logger::warn("failed to add actor to the world: actor is already in the world: ",
                         typeid(*pActor).name());
            return GameObjectHandle<TActor>();
        }

        pActor->bAddedToWorld = true;

        auto* pRootComp = pActor->getRoot();

        LEMON_ASSERT(pRootComp != nullptr, "actor must have root component assigned before spawning");

        pRootComp->setLocalPosition(pos);
        pRootComp->setLocalRotation(rot);
        pRootComp->setLocalScale(scale);

        // TODO: Make sure that `onStart()` runs only when it's safe.
        pActor->start();

        return pActor;
    }
} // namespace lemon::game
