#include "Actor.h"
#include "GameWorld.h"

using namespace lemon;
using namespace lemon::game;

Actor::Actor() : GameObject()
{
    LEMON_TRACE_FN();

    tick.setTickType(GameObjectTickType::Actor);
}

Actor::~Actor()
{
    LEMON_TRACE_FN();

    uninitializeComponents();
    unregisterComponents();
}

PositionableComponent*
Actor::getRootComponent()
{
    return rootComponent;
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
Actor::startInternal()
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
Actor::stopInternal()
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
