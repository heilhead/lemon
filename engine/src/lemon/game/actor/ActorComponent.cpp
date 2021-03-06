#include <lemon/game/actor/ActorComponent.h>
#include <lemon/game/actor/Actor.h>
#include <lemon/game/actor/GameWorld.h>

using namespace lemon;
using namespace lemon::game;

ActorComponent::ActorComponent() : GameObject()
{
    tick.setGroup(GameWorld::get()->getComponentTickGroup());
}

ActorComponent::~ActorComponent() {}

void
ActorComponent::enableTick(float interval)
{
    if (!tick.isEnabled()) {
        GameObject::enableTick(interval);

        auto* pTickingParent = findTickingParent();
        if (pTickingParent != nullptr) {
            tick.setTickingParent(pTickingParent->tick.getHandle());
        }

        updateTickRecursive(this, tick.getHandle());
    }
}

void
ActorComponent::disableTick()
{
    if (tick.isEnabled()) {
        GameObject::disableTick();

        auto* pTickingParent = findTickingParent();
        if (pTickingParent != nullptr) {
            updateTickRecursive(this, pTickingParent->tick.getHandle());
        } else {
            updateTickRecursive(this, TickProxyHandle());
        }
    }
}

void
ActorComponent::addTickDependency(GameObject* pOtherObject)
{
    if (auto* pComponent = cast<ActorComponent>(pOtherObject)) {
        GameObject::addTickDependency(pOtherObject);
    } else {
        logger::warn("failed to add tick dependency: actor component may only have other actor components as "
                     "tick dependencies");
    }
}

void
ActorComponent::removeTickDependency(GameObject* pOtherObject)
{
    if (auto* pComponent = cast<ActorComponent>(pOtherObject)) {
        GameObject::removeTickDependency(pOtherObject);
    } else {
        logger::warn("failed to remove tick dependency: actor component may only have other actor components "
                     "as tick dependencies");
    }
}

PositionableComponent*
ActorComponent::getRoot()
{
    return getOwner()->getRoot();
}

const PositionableComponent*
ActorComponent::getRoot() const
{
    return getOwner()->getRoot();
}

void
ActorComponent::onRegister()
{
}

void
ActorComponent::onInitialize()
{
}

void
ActorComponent::onUninitialize()
{
}

void
ActorComponent::onUnregister()
{
}

void
ActorComponent::setOwner(Actor* pInOwner)
{
    pOwner = pInOwner;
}

void
ActorComponent::updateTickRecursive(GameObject* pParent, TickProxyHandle newHandle)
{
    pParent->iterateSubObjects([&](GameObject* pObject) {
        if (auto* pComponent = pObject->cast<ActorComponent>()) {
            if (pComponent->isTickEnabled()) {
                pComponent->tick.setTickingParent(newHandle);
            } else {
                updateTickRecursive(pComponent, newHandle);
            }
        }
    });
}

inline ActorComponent*
ActorComponent::findTickingParent()
{
    auto* pParentObj = getParent();
    while (pParentObj != nullptr) {
        if (pParentObj->isTickEnabled()) {
            auto* pParentComp = cast<ActorComponent>(pParentObj);
            if (pParentComp != nullptr) {
                return pParentComp;
            }
        }

        pParentObj = pParentObj->getParent();
    }

    return nullptr;
}

void
PositionableComponent::onRegister()
{
}

void
PositionableComponent::onInitialize()
{
}

glm::f32vec3
PositionableComponent::getLocalPosition() const
{
    return transform.getPosition();
}

glm::quat
PositionableComponent::getLocalRotation() const
{
    return transform.getRotation();
}

glm::f32vec3
PositionableComponent::getLocalScale() const
{
    return transform.getScale();
}

void
PositionableComponent::setLocalPosition(const glm::f32vec3& pos)
{
    transform.setPosition(pos);
    transformCache.bDirty = true;
}

void
PositionableComponent::setLocalRotation(const glm::quat& rot)
{
    transform.setRotation(rot);
    transformCache.bDirty = true;
}

void
PositionableComponent::setLocalScale(const glm::f32vec3& scale)
{
    transform.setScale(scale);
    transformCache.bDirty = true;
}

inline void
PositionableComponent::updateLocalTransform() const
{
    if (transformCache.bDirty) {
        transformCache.localMatrix = transform.getMatrix();
        transformCache.generation++;
        transformCache.bDirty = false;
    }
}

uint32_t
PositionableComponent::updateGlobalTransform() const
{
    updateLocalTransform();

    auto* pParent = getParent<PositionableComponent>();
    if (pParent == nullptr) {
        return transformCache.generation;
    }

    pParent->updateGlobalTransform();

    auto parentGeneration = pParent->transformCache.generation;
    if (parentGeneration == transformCache.parentGeneration) {
        return transformCache.generation;
    }

    transformCache.globalMatrix = pParent->transformCache.globalMatrix * transformCache.localMatrix;
    transformCache.parentGeneration = parentGeneration;
    transformCache.generation++;

    return transformCache.generation;
}

const glm::f32mat4&
PositionableComponent::getLocalTransformMatrix() const
{
    updateLocalTransform();
    return transformCache.localMatrix;
}

const glm::f32mat4&
PositionableComponent::getGlobalTransformMatrix() const
{
    updateGlobalTransform();

    if (getParent() != nullptr) {
        return transformCache.globalMatrix;
    } else {
        return transformCache.localMatrix;
    }
}

GameObjectRenderProxy::GameObjectRenderProxy(RenderableComponent* pRenderable) : pRenderable{pRenderable} {}

void
RenderableComponent::onStart()
{
    PositionableComponent::onStart();
    renderProxyHandle = GameWorld::get()->registerRenderableComponentInternal(createRenderProxy());
}

void
RenderableComponent::onStop()
{
    GameWorld::get()->unregisterRenderableComponentInternal(renderProxyHandle);
    PositionableComponent::onStop();
}

GameObjectRenderProxy
RenderableComponent::createRenderProxy()
{
    return GameObjectRenderProxy(this);
}

void
RenderableComponent::updateRenderProxy()
{
    auto* pRenderProxy = GameWorld::get()->getRenderProxy(renderProxyHandle);
    LEMON_ASSERT(pRenderProxy != nullptr);
}
