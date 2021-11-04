#include <lemon/game/actor/GameObject.h>
#include <lemon/game/actor/GameObjectStore.h>
#include <lemon/game/actor/GameWorld.h>

using namespace lemon;
using namespace lemon::game;

GameObjectTickDescriptor::GameObjectTickDescriptor(GameObject* pOwner)
    : pGroup{nullptr}, pOwner{pOwner}, dependencies{}, handle{}, interval{0.f}, bEnabled{false}
{
}

GameObjectTickDescriptor::~GameObjectTickDescriptor()
{
    while (dependencies.size()) {
        removeDependency(dependencies.back());
    }

    while (dependants.size()) {
        dependants.back()->getTickDescriptor().removeDependency(pOwner);
    }
}

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
GameObjectTickDescriptor::addDependency(GameObject* pObject)
{
    dependencies.erase(std::remove(dependencies.begin(), dependencies.end(), pObject), dependencies.end());
    dependencies.push_back(pObject);

    pObject->getTickDescriptor().addDependant(pOwner);

    updateProxy();
}

void
GameObjectTickDescriptor::removeDependency(GameObject* pObject)
{
    dependencies.erase(std::remove(dependencies.begin(), dependencies.end(), pObject), dependencies.end());

    pObject->getTickDescriptor().removeDependant(pOwner);

    updateProxy();
}

void
GameObjectTickDescriptor::addDependant(GameObject* pObject)
{
    dependants.erase(std::remove(dependants.begin(), dependants.end(), pObject), dependants.end());
    dependants.push_back(pObject);
}

void
GameObjectTickDescriptor::removeDependant(GameObject* pObject)
{
    dependants.erase(std::remove(dependants.begin(), dependants.end(), pObject), dependants.end());
}

void
GameObjectTickDescriptor::setTickingParent(TickProxyHandle handle)
{
    getProxy()->tickingParent = handle;
}

void
GameObjectTickDescriptor::setHandle(TickProxyHandle inHandle)
{
    handle = inHandle;
}

TickProxyHandle
GameObjectTickDescriptor::getHandle() const
{
    return handle;
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
    return pGroup;
}

const TickGroup*
GameObjectTickDescriptor::getGroup() const
{
    return pGroup;
}

void
GameObjectTickDescriptor::setGroup(TickGroup* pInGroup)
{
    pGroup = pInGroup;
}

TickProxy*
GameObjectTickDescriptor::getProxy()
{
    return pGroup->getProxy(getHandle());
}

const TickProxy*
GameObjectTickDescriptor::getProxy() const
{
    return pGroup->getProxy(getHandle());
}

void
GameObjectTickDescriptor::enable(float interval)
{
    if (!bEnabled) {
        bEnabled = true;
        setHandle(pGroup->add(TickProxy(pOwner, interval, dependencies.size())));
    }
}

void
GameObjectTickDescriptor::disable()
{
    if (bEnabled) {
        bEnabled = false;
        pGroup->remove(getHandle());
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
    if (auto* pProxy = getProxy()) {
        pProxy->dependencyCount = dependencies.size();
    }
}

GameObject::GameObject() : tick{this} {}

GameObject::~GameObject()
{
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

GameObjectStoreHandle
GameObject::getStoreHandle() const
{
    return objectDescriptor.storeHandle;
}

void
GameObject::enableTick(float interval)
{
    tick.enable(interval);
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

GameObjectTickDescriptor&
GameObject::getTickDescriptor()
{
    return tick;
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
GameObject::setName(const std::string& name)
{
    objectName = name;
}

const std::string&
GameObject::getName() const
{
    return objectName;
}

void
GameObject::addTickDependency(GameObject* pOther)
{
    tick.addDependency(pOther);
}

void
GameObject::removeTickDependency(GameObject* pOther)
{
    tick.removeDependency(pOther);
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
}

void
GameObject::onTick(float deltaTime)
{
}

void
GameObject::onStop()
{
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
