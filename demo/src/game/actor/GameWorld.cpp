#include "GameWorld.h"

using namespace lemon;
using namespace lemon::game;

GameWorld::GameWorld()
    : store{}, actors{kMaxAliveGameObjects}, tickingActors{}, tickingComponents{}, renderableComponents{512},
      lastUpdateTime{0.f}
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

    pActor->stop();
}

GameObjectRenderProxy*
GameWorld::getRenderProxy(RenderProxyHandle handle)
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
    lastUpdateTime = time;
    tickingActors.tick(time);
    tickingComponents.tick(time);
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

RenderProxyHandle
GameWorld::registerRenderableComponentInternal(const GameObjectRenderProxy& proxy)
{
    LEMON_ASSERT(proxy.pRenderable != nullptr);
    return renderableComponents.insert(proxy);
}

void
GameWorld::unregisterRenderableComponentInternal(RenderProxyHandle handle)
{
    renderableComponents.remove(handle);
}

TickGroup*
GameWorld::getActorTickGroup()
{
    return &tickingActors;
}

TickGroup*
GameWorld::getComponentTickGroup()
{
    return &tickingComponents;
}
