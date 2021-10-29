#include "GameObject.h"
#include "GameObjectStore.h"
#include "GameWorld.h"

using namespace lemon;
using namespace lemon::game;

GameObject::~GameObject()
{
    LEMON_TRACE_FN();

    auto* pStore = GameObjectStore::get();
    for (auto* pObject : getSubObjectList()) {
        pStore->destroy(pObject);
    }
}

const GameObjectDescriptor&
GameObject::getObjectDescriptor() const
{
    return objectDescriptor;
}

const GameObject*
GameObject::getParent() const
{
    return pParent;
};

GameObject*
GameObject::getParent()
{
    return pParent;
};

GameObjectInternalHandle
GameObject::getInternalHandle() const
{
    return objectDescriptor.storeHandle;
}

void
GameObject::enableTick()
{
    if (!bTickEnabled) {
        switch (tick.getTickType()) {
        case GameObjectTickType::Actor:
            tick.setHandle(GameWorld::get()->registerTickingActor(cast<Actor>()));
            break;
        case GameObjectTickType::Component:
            tick.setHandle(GameWorld::get()->registerTickingComponent(cast<ActorComponent>()));
            break;
        default:
            LEMON_UNREACHABLE();
        }

        bTickEnabled = true;
    }
}

void
GameObject::disableTick()
{
    if (bTickEnabled) {
        switch (tick.getTickType()) {
        case GameObjectTickType::Actor:
            GameWorld::get()->unregisterTickingActor(tick.getHandle());
            break;
        case GameObjectTickType::Component:
            GameWorld::get()->unregisterTickingComponent(tick.getHandle());
            break;
        default:
            LEMON_UNREACHABLE();
        }

        bTickEnabled = false;
    }
}

void
GameObject::iterateSubObjects(const std::function<void(GameObject*)>& fn, bool bRecursive)
{
    for (auto* pObject : subObjects) {
        fn(pObject);

        if (bRecursive) {
            pObject->iterateSubObjects(fn, true);
        }
    }
}

void
GameObject::iterateSubObjects(const std::function<void(const GameObject*)>& fn, bool bRecursive) const
{
    for (const auto* pObject : subObjects) {
        fn(pObject);

        if (bRecursive) {
            pObject->iterateSubObjects(fn, true);
        }
    }
}

void
GameObject::setParent(GameObject* inParent)
{
    pParent = inParent;
}

const GameObject::SubObjectList&
GameObject::getSubObjectList() const
{
    return subObjects;
}

GameObject::SubObjectList&
GameObject::getSubObjectList()
{
    return subObjects;
}
