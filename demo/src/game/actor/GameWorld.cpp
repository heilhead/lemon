#include "GameWorld.h"

using namespace lemon;
using namespace lemon::game;

GameWorld::GameWorld()
    : store{}, actors{}, tickingActors{}, tickingComponents{}, renderableComponents{}, lastUpdateTime{0.f}
{
}

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
GameWorld::resolveTickableObject(GameObjectTickProxyHandle handle, GameObjectTickType tickType)
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

GameObjectTickProxy*
GameWorld::getTickProxy(GameObjectTickProxyHandle handle, GameObjectTickType tickType)
{
    switch (tickType) {
    case GameObjectTickType::Actor:
        return tickingActors.getData(handle);

    case GameObjectTickType::Component:
        return tickingComponents.getData(handle);

    default:
        LEMON_UNREACHABLE();
    }

    return nullptr;
}

GameObjectRenderProxy*
GameWorld::getRenderProxy(GameObjectRenderProxyHandle handle)
{
    return renderableComponents.getData(handle);
}

GameObjectStore*
GameWorld::getStoreInternal()
{
    return &get()->store;
}

void
GameWorld::updateInternal(double time)
{
    auto dt = static_cast<float>(time - lastUpdateTime);

    for (size_t i = 0, length = tickingActors.getSize(); i < length; i++) {
        tick(tickingActors[i], time, dt);
    }

    for (size_t i = 0, length = tickingComponents.getSize(); i < length; i++) {
        tick(tickingComponents[i], time, dt);
    }
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
GameWorld::registerRenderableComponentInternal(const GameObjectRenderProxy& proxy)
{
    LEMON_ASSERT(proxy.pRenderable != nullptr);
    return renderableComponents.insert(proxy);
}

void
GameWorld::unregisterRenderableComponentInternal(GameObjectRenderProxyHandle handle)
{
    renderableComponents.remove(handle);
}

inline void
GameWorld::tick(GameObjectTickProxy& proxy, double time, float dt)
{
    bool bShouldTick = (time != proxy.lastTickTime) && (time - proxy.lastTickTime >= proxy.interval);

    if (bShouldTick) {
        auto* pObject = proxy.pObject;

        if (proxy.dependencyCount > 0) {
            auto& tickDesc = pObject->getTickDescriptor();

            for (auto hDep : tickDesc.getDependencies()) {
                auto* depProxy = getTickProxy(hDep, tickDesc.getTickType());

                LEMON_ASSERT(depProxy != nullptr);

                tick(*depProxy, time, dt);
            }
        }

        proxy.pObject->onTick(dt);
        proxy.lastTickTime = time;
    }
}
