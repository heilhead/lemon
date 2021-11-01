#include "Actor.h"
#include "GameWorld.h"

using namespace lemon;
using namespace lemon::game;

Actor::Actor() : GameObject()
{
    LEMON_TRACE_FN();

    tick.setGroup(GameWorld::get()->getActorTickGroup());
}

Actor::~Actor()
{
    LEMON_TRACE_FN();

    uninitializeComponents();
    unregisterComponents();
}

PositionableComponent*
Actor::getRoot()
{
    return root;
}

const PositionableComponent*
Actor::getRoot() const
{
    return root;
}

void
Actor::onPreInitializeComponents()
{
    LEMON_TRACE_FN();
}

void
Actor::onPostInitializeComponents()
{
    LEMON_TRACE_FN();
}

void
Actor::onStart()
{
    GameObject::onStart();
    LEMON_TRACE_FN();
    worldHandle = GameWorld::get()->registerActorInternal(this);
}

void
Actor::onStop()
{
    LEMON_TRACE_FN();
    GameWorld::get()->unregisterActorInternal(worldHandle);
    GameObject::onStop();
}

void
Actor::addTickDependency(GameObject* pOtherObject)
{
    if (auto* pComponent = cast<Actor>(pOtherObject)) {
        GameObject::addTickDependency(pOtherObject);
    } else {
        logger::warn("Failed to add tick dependency: actor may only have other actors as tick dependencies");
    }
}

void
Actor::removeTickDependency(GameObject* pOtherObject)
{
    if (auto* pComponent = cast<Actor>(pOtherObject)) {
        GameObject::removeTickDependency(pOtherObject);
    } else {
        logger::warn("Failed to add tick dependency: actor may only have other actors as tick dependencies");
    }
}

void
Actor::seal()
{
    bSealed = true;
}

void
Actor::start()
{
    if (!bComponentsInitialized) {
        onPreInitializeComponents();
        initializeComponents();
        onPostInitializeComponents();

        bComponentsInitialized = true;
    }

    iterateSubObjects(
        [](GameObject* pObject) {
            if (auto* pComponent = game::cast<ActorComponent>(pObject)) {
                pComponent->onStart();
            }
        },
        true);

    bAddedToWorld = true;
    onStart();
}

void
Actor::stop()
{
    iterateSubObjects(
        [](GameObject* pObject) {
            if (auto* pComponent = game::cast<ActorComponent>(pObject)) {
                pComponent->onStop();
            }
        },
        true);

    onStop();
    bAddedToWorld = false;
}

void
Actor::initializeComponents()
{
    iterateSubObjects(
        [](GameObject* pObject) {
            if (auto* pComponent = game::cast<ActorComponent>(pObject)) {
                pComponent->onInitialize();
            }
        },
        true);
}

void
Actor::uninitializeComponents()
{
    iterateSubObjects(
        [](GameObject* pObject) {
            if (auto* pComponent = game::cast<ActorComponent>(pObject)) {
                pComponent->onUninitialize();
            }
        },
        true);
}

void
Actor::unregisterComponents()
{
    iterateSubObjects(
        [](GameObject* pObject) {
            if (auto* pComponent = game::cast<ActorComponent>(pObject)) {
                pComponent->onUnregister();
            }
        },
        true);
}
