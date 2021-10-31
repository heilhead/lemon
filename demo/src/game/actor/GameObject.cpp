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
    auto* pNewObject = group->getProxy(handle)->pObject;

    LEMON_ASSERT(pNewObject != nullptr);

    auto predicate = [&](auto hExisting) {
        auto* pExistingObject = group->getProxy(hExisting)->pObject;

        LEMON_ASSERT(pExistingObject != nullptr);

        return pExistingObject->isParentOf(pNewObject);
    };

    // Make sure to remove existing dependencies which are parents of the new dependency.
    dependencies.erase(std::remove_if(dependencies.begin(), dependencies.end(), predicate),
                       dependencies.end());

    dependencies.push_back(handle);

    updateProxy();
}

void
GameObjectTickDescriptor::removeDependency(ProxyHandle handle)
{
    dependencies.erase(std::remove(dependencies.begin(), dependencies.end(), handle), dependencies.end());

    updateProxy();
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
    if (isEnabled()) {
        return getProxy()->interval;
    }

    return 0.f;
}

void
GameObjectTickDescriptor::setInterval(float value)
{
    if (isEnabled()) {
        getProxy()->interval = value;
    }
}

TickGroup*
GameObjectTickDescriptor::getGroup()
{
    return group;
}

const TickGroup*
GameObjectTickDescriptor::getGroup() const
{
    return group;
}

void
GameObjectTickDescriptor::setGroup(TickGroup* inGroup)
{
    group = inGroup;
}

GameObjectTickProxy*
GameObjectTickDescriptor::getProxy()
{
    return group->getProxy(getHandle());
}

const GameObjectTickProxy*
GameObjectTickDescriptor::getProxy() const
{
    return group->getProxy(getHandle());
}

void
GameObjectTickDescriptor::enable(GameObject* pObject, float interval)
{
    if (!bEnabled) {
        bEnabled = true;
        setHandle(group->add(GameObjectTickProxy(pObject, interval, dependencies.size())));
    }
}

void
GameObjectTickDescriptor::disable()
{
    if (bEnabled) {
        bEnabled = false;
        group->remove(getHandle());
    }
}

bool
GameObjectTickDescriptor::isEnabled() const
{
    return bEnabled;
}

inline void
GameObjectTickDescriptor::updateProxy()
{
    auto* pProxy = getProxy();
    LEMON_ASSERT(pProxy != nullptr);
    pProxy->dependencyCount = dependencies.size();
}

GameObjectTickProxy::GameObjectTickProxy(GameObject* pObject, float interval, uint32_t dependencyCount)
    : pObject{pObject}, interval{interval}, lastTickTime{0.f}, dependencyCount{dependencyCount}
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

GameObjectStoreHandle
GameObject::getInternalHandle() const
{
    return objectDescriptor.storeHandle;
}

void
GameObject::enableTick(float interval)
{
    tick.enable(this, interval);
}

void
GameObject::disableTick()
{
    tick.disable();
}

bool
GameObject::isTickEnabled() const
{
    return tick.isEnabled();
}

const GameObjectTickDescriptor&
GameObject::getTickDescriptor() const
{
    return tick;
}

void
GameObject::addTickDependencyInternal(TickGroupHandle handle)
{
    tick.addDependency(handle);
}

void
GameObject::removeTickDependencyInternal(TickGroupHandle handle)
{
    tick.removeDependency(handle);
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

    tick.disable();
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
