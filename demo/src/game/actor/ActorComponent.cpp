#include "ActorComponent.h"
#include "Actor.h"
#include "GameWorld.h"

using namespace lemon;
using namespace lemon::game;

ActorComponent::ActorComponent() : GameObject()
{
    LEMON_TRACE_FN();

    tick.setTickType(GameObjectTickType::Component);
}

ActorComponent::~ActorComponent()
{
    LEMON_TRACE_FN();
}

void
ActorComponent::enableTick(float interval)
{
    if (!bTickEnabled) {
        GameObject::enableTick(interval);

        auto* pTickingParent = findTickingParent();
        if (pTickingParent != nullptr) {
            addTickDependencyInternal(pTickingParent->tick.getHandle());
        }

        attachTickRecursive(this);
    }
}

void
ActorComponent::disableTick()
{
    if (bTickEnabled) {
        GameObject::disableTick();

        auto* pTickingParent = findTickingParent();
        if (pTickingParent != nullptr) {
            removeTickDependencyInternal(pTickingParent->tick.getHandle());
        }
    }
}

void
ActorComponent::onRegister()
{
    LEMON_TRACE_FN();
}

void
ActorComponent::onInitialize()
{
    LEMON_TRACE_FN();
}

void
ActorComponent::onUninitialize()
{
    LEMON_TRACE_FN();
}

void
ActorComponent::onUnregister()
{
    LEMON_TRACE_FN();
}

void
ActorComponent::attachTickRecursive(GameObject* pParent)
{
    pParent->iterateSubObjects([&](GameObject* pObject) {
        if (auto* pComponent = pObject->cast<ActorComponent>()) {
            if (pComponent->isTickEnabled()) {
                pComponent->tick.addDependency(tick.getHandle());
            } else {
                attachTickRecursive(pComponent);
            }
        }
    });
}

void
ActorComponent::detachTickRecursive(GameObject* pParent)
{
    pParent->iterateSubObjects([&](GameObject* pObject) {
        if (auto* pComponent = pObject->cast<ActorComponent>()) {
            if (pComponent->isTickEnabled()) {
                pComponent->tick.removeDependency(tick.getHandle());
            } else {
                detachTickRecursive(pComponent);
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
    LEMON_TRACE_FN();

    auto* pParentObj = getParent();
    while (pParentObj != nullptr) {
        if (auto* pPositionableParent = cast<PositionableComponent>(pParentObj)) {
            transformCache.pParent = pPositionableParent;

            break;
        }

        if (auto* pActorParent = cast<Actor>(pParentObj)) {
            auto* pRootComp = pActorParent->getRootComponent();

            if (pRootComp != this) {
                transformCache.pParent = pRootComp;
            }

            break;
        }

        pParentObj = pParentObj->getParent();
    }
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

inline void
PositionableComponent::updateGlobalTransform() const
{
    updateLocalTransform();

    const auto* pParent = transformCache.pParent;

    if (pParent != nullptr) {
        return;
    }

    pParent->updateGlobalTransform();

    auto parentGeneration = pParent->transformCache.generation;
    if (parentGeneration == transformCache.parentGeneration) {
        return;
    }

    transformCache.globalMatrix = pParent->transformCache.globalMatrix * transformCache.localMatrix;
    transformCache.parentGeneration = parentGeneration;
    transformCache.generation++;
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
    return transformCache.pParent != nullptr ? transformCache.globalMatrix : transformCache.localMatrix;
}

GameObjectRenderProxy::GameObjectRenderProxy(RenderableComponent* pRenderable) : pRenderable{pRenderable} {}

void
RenderableComponent::onStart()
{
    PositionableComponent::onStart();
    LEMON_TRACE_FN();
    renderProxyHandle = GameWorld::get()->registerRenderableComponentInternal(createRenderProxy());
}

void
RenderableComponent::onStop()
{
    LEMON_TRACE_FN();
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
