#include "GameObject.h"
#include "GameObjectStore.h"
#include "GameWorld.h"

using namespace lemon;
using namespace lemon::game;

GameObjectTickDescriptor::Dependencies&
GameObjectTickDescriptor::getDependencies()
{
    return dependencies;
}

const GameObjectTickDescriptor::Dependencies&
GameObjectTickDescriptor::getDependencies() const
{
    return dependencies;
}

void
GameObjectTickDescriptor::addDependency(ProxyHandle handle)
{
    auto* pWorld = GameWorld::get();
    auto* pNewObject = pWorld->resolveTickableObject(handle, tickType);

    LEMON_ASSERT(pNewObject != nullptr);

    auto predicate = [&](auto hExisting) {
        auto* pExistingObject = pWorld->resolveTickableObject(hExisting, tickType);

        LEMON_ASSERT(pExistingObject != nullptr);

        return pExistingObject->isParentOf(pNewObject);
    };

    // Make sure to remove existing dependencies which are parents of the new dependency.
    dependencies.erase(std::remove_if(dependencies.begin(), dependencies.end(), predicate),
                       dependencies.end());

    dependencies.push_back(handle);
}

void
GameObjectTickDescriptor::removeDependency(ProxyHandle handle)
{
    dependencies.erase(std::remove(dependencies.begin(), dependencies.end(), handle), dependencies.end());
}

GameObjectTickType
GameObjectTickDescriptor::getTickType() const
{
    return tickType;
}

void
GameObjectTickDescriptor::setTickType(GameObjectTickType inTickType)
{
    tickType = inTickType;
}

void
GameObjectTickDescriptor::setHandle(ProxyHandle handle)
{
    tickProxyHandle = handle;
}

GameObjectTickDescriptor::ProxyHandle
GameObjectTickDescriptor::getHandle() const
{
    return tickProxyHandle;
}

float
GameObjectTickDescriptor::getInterval() const
{
    return interval;
}

void
GameObjectTickDescriptor::setInterval(float value)
{
    interval = value;
}

GameObjectTickProxy::GameObjectTickProxy(GameObject* pObject, float interval)
    : pObject{pObject}, interval{interval}, lastTickTime{0.f}
{
}

GameObject::GameObject()
{
    LEMON_TRACE_FN();
}

GameObject::~GameObject()
{
    LEMON_TRACE_FN();

    auto* pStore = GameWorld::getStoreInternal();
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
GameObject::enableTick(float interval)
{
    if (!bTickEnabled) {
        tick.setInterval(interval);
        tick.setHandle(
            GameWorld::get()->registerTickingObjectInternal(createTickProxy(), tick.getTickType()));
        bTickEnabled = true;
    }
}

void
GameObject::disableTick()
{
    if (bTickEnabled) {
        GameWorld::get()->unregisterTickingObjectInternal(tick.getHandle(), tick.getTickType());
        bTickEnabled = false;
    }
}

bool
GameObject::isTickEnabled() const
{
    return bTickEnabled;
}

const GameObjectTickDescriptor&
GameObject::getTickDescriptor() const
{
    return tick;
}

void
GameObject::addTickDependencyInternal(GameObjectTickProxyHandle handle)
{
    tick.addDependency(handle);
    updateTickProxy();
}

void
GameObject::removeTickDependencyInternal(GameObjectTickProxyHandle handle)
{
    tick.removeDependency(handle);
    updateTickProxy();
}

bool
GameObject::isParentOf(const GameObject* pObject) const
{
    if (pObject == this) {
        return false;
    }

    auto* pParentObj = pObject->getParent();
    while (pParentObj != nullptr) {
        if (pParentObj == this) {
            return true;
        }

        pParentObj = pParentObj->getParent();
    }

    return false;
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
GameObject::onStart()
{
    LEMON_TRACE_FN();
}

void
GameObject::onTick(float deltaTime)
{
    LEMON_TRACE_FN();
}

void
GameObject::onStop()
{
    LEMON_TRACE_FN();

    if (bTickEnabled) {
        disableTick();
    }
}

void
GameObject::setParent(GameObject* inParent)
{
    pParent = inParent;
}

GameObjectTickProxy
GameObject::createTickProxy()
{
    return GameObjectTickProxy(this, tick.getInterval());
}

inline void
GameObject::updateTickProxy()
{
    auto* pProxy = GameWorld::get()->getTickProxy(tick.getHandle(), tick.getTickType());
    LEMON_ASSERT(pProxy != nullptr);
    pProxy->dependencyCount = tick.getDependencies().size();
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
