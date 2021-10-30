#include "GameWorld.h"

using namespace lemon;
using namespace lemon::game;

void
GameWorld::removeActor(Actor* pActor)
{
    if (!pActor->bAddedToWorld) {
        logger::warn("Failed to remove actor from the world: actor is not in the world: ",
                     typeid(*pActor).name());
        return;
    }

    pActor->stopInternal();
}

GameObject*
GameWorld::resolveTickableObject(GameObjectTickProxyHandle handle, GameObjectTickType tickType) const
{
    switch (tickType) {
    case GameObjectTickType::Actor: {
        auto* pData = tickingActors.getData(handle);
        if (pData != nullptr) {
            return pData->pObject;
        }

        return nullptr;
    }

    case GameObjectTickType::Component: {
        auto* pData = tickingComponents.getData(handle);
        if (pData != nullptr) {
            return pData->pObject;
        }

        return nullptr;
    }

    default:
        LEMON_UNREACHABLE();
    }

    return nullptr;
}

GameObjectStore*
GameWorld::getStoreInternal()
{
    return &get()->store;
}

GameObjectWorldHandle
GameWorld::registerActorInternal(Actor* pActor)
{
    LEMON_ASSERT(pActor != nullptr);
    return actors.insert(pActor);
}

void
GameWorld::unregisterActorInternal(GameObjectWorldHandle handle)
{
    actors.remove(handle);
}

GameObjectTickProxyHandle
GameWorld::registerTickingObjectInternal(const GameObjectTickProxy& tick, GameObjectTickType tickType)
{
    switch (tickType) {
    case GameObjectTickType::Actor:
        return tickingActors.insert(tick);
    case GameObjectTickType::Component:
        return tickingComponents.insert(tick);
    default:
        LEMON_UNREACHABLE();
    }

    return GameObjectTickProxyHandle();
}

void
GameWorld::unregisterTickingObjectInternal(GameObjectTickProxyHandle handle, GameObjectTickType tickType)
{
    switch (tickType) {
    case GameObjectTickType::Actor:
        tickingActors.remove(handle);
        break;
    case GameObjectTickType::Component:
        tickingComponents.remove(handle);
        break;
    default:
        LEMON_UNREACHABLE();
    }
}

GameObjectRenderProxyHandle
GameWorld::registerRenderableComponentInternal(RenderableComponent* pComponent)
{
    LEMON_ASSERT(pComponent != nullptr);
    return renderableComponents.insert(pComponent);
}

void
GameWorld::unregisterRenderableComponentInternal(GameObjectRenderProxyHandle handle)
{
    renderableComponents.remove(handle);
}
