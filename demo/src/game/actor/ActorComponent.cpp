#include "ActorComponent.h"
#include "Actor.h"
#include "GameWorld.h"

using namespace lemon;
using namespace lemon::game;

ActorComponent::~ActorComponent()
{
    LEMON_TRACE_FN();
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

void
RenderableComponent::onStart()
{
    PositionableComponent::onStart();
    LEMON_TRACE_FN();
    renderQueueHandle = GameWorld::get()->registerRenderableComponent(this);
}

void
RenderableComponent::onStop()
{
    LEMON_TRACE_FN();
    GameWorld::get()->unregisterRenderableComponent(renderQueueHandle);
    PositionableComponent::onStop();
}
