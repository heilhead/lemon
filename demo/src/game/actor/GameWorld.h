#pragma once

#include "ActorComponent.h"
#include "Actor.h"
#include "GameObjectStore.h"
#include "TickGroup.h"

namespace lemon::game {
    class GameWorld : public UnsafeSingleton<GameWorld> {
        GameObjectStore store;

        SlotMap<Actor*, GameObjectWorldHandle> actors;
        TickGroup tickingActors;
        TickGroup tickingComponents;
        SlotMap<GameObjectRenderProxy, GameObjectRenderProxyHandle> renderableComponents;

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
        getRenderProxy(GameObjectRenderProxyHandle handle);

        static GameObjectStore*
        getStoreInternal();

        void
        updateInternal(double time);

        GameObjectWorldHandle
        registerActorInternal(Actor* pActor);

        void
        unregisterActorInternal(GameObjectWorldHandle handle);

        GameObjectRenderProxyHandle
        registerRenderableComponentInternal(const GameObjectRenderProxy& proxy);

        void
        unregisterRenderableComponentInternal(GameObjectRenderProxyHandle handle);

        TickGroup*
        getActorTickGroup();

        TickGroup*
        getComponentTickGroup();

    private:
        void
        tick(GameObjectTickProxy& proxy, double time, float dt);
    };

    template<ActorBase TActor>
    GameObjectHandle<TActor>
    GameWorld::createActor(const glm::f32vec3& pos, const glm::quat& rot, const glm::f32vec3& scale)
    {
        return addActor(store.create<TActor>(), pos, rot, scale);
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
            logger::warn("Failed to add actor to the world: actor is already in the world: ",
                         typeid(*pActor).name());
            return GameObjectHandle<TActor>();
        }

        pActor->bAddedToWorld = true;

        auto* pRootComp = pActor->getRootComponent();

        LEMON_ASSERT(pRootComp != nullptr, "actor must have root component assigned before spawning");

        pRootComp->setLocalPosition(pos);
        pRootComp->setLocalRotation(rot);
        pRootComp->setLocalScale(scale);

        pActor->startInternal();

        return pActor;
    }
} // namespace lemon::game
