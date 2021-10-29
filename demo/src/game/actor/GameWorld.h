#pragma once

#include "ActorComponent.h"
#include "Actor.h"
#include "GameObjectStore.h"

namespace lemon::game {
    class GameWorld : public UnsafeSingleton<GameWorld> {
        friend class GameObject;
        friend class Actor;
        friend class ActorComponent;
        friend class RenderableComponent;

        // TODO: SlotMap should probably by dynamically-sized?
        SlotMap<Actor*, kMaxAliveGameObjects> actors;
        SlotMap<Actor*, kMaxAliveGameObjects> tickingActors;
        SlotMap<ActorComponent*, kMaxAliveGameObjects> tickingComponents;
        SlotMap<RenderableComponent*, kMaxAliveGameObjects> renderableComponents;

    public:
        template<ActorBase TActor>
        GameObjectHandle<TActor>
        createActor(const glm::f32vec3& pos = kVectorZero, const glm::quat& rot = kQuatIdentity,
                    const glm::f32vec3& scale = kVectorOne)
        {
            return addActor(GameObjectStore::get()->create<TActor>(), pos, rot, scale);
        }

        template<ActorBase TActor>
        void
        destroyActor(GameObjectHandle<TActor> handle)
        {
            auto* pActor = handle.get();

            if (pActor->bAddedToWorld) {
                removeActor(pActor);
            }

            GameObjectStore::get()->destroy(pActor);
        }

        template<ActorBase TActor>
        GameObjectHandle<TActor>
        addActor(TActor* pActor, const glm::f32vec3& pos = kVectorZero, const glm::quat& rot = kQuatIdentity,
                 const glm::f32vec3& scale = kVectorOne)
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

        void
        removeActor(Actor* pActor)
        {
            if (!pActor->bAddedToWorld) {
                logger::warn("Failed to remove actor from the world: actor is not in the world: ",
                             typeid(*pActor).name());
                return;
            }

            pActor->stopInternal();
        }

    private:
        SlotMapHandle
        registerActor(Actor* pActor)
        {
            LEMON_ASSERT(pActor != nullptr);
            return actors.insert(pActor);
        }

        void
        unregisterActor(SlotMapHandle handle)
        {
            actors.remove(handle);
        }

        SlotMapHandle
        registerTickingActor(Actor* pActor)
        {
            LEMON_ASSERT(pActor != nullptr);
            return tickingActors.insert(pActor);
        }

        void
        unregisterTickingActor(SlotMapHandle handle)
        {
            tickingActors.remove(handle);
        }

        SlotMapHandle
        registerTickingComponent(ActorComponent* pComponent)
        {
            LEMON_ASSERT(pComponent != nullptr);
            return tickingComponents.insert(pComponent);
        }

        void
        unregisterTickingComponent(SlotMapHandle handle)
        {
            tickingComponents.remove(handle);
        }

        SlotMapHandle
        registerRenderableComponent(RenderableComponent* pComponent)
        {
            LEMON_ASSERT(pComponent != nullptr);
            return renderableComponents.insert(pComponent);
        }

        void
        unregisterRenderableComponent(SlotMapHandle handle)
        {
            renderableComponents.remove(handle);
        }
    };
} // namespace lemon::game
