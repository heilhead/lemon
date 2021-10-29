#include "Actor.h"

using namespace lemon;
using namespace lemon::game;

Actor::~Actor()
{
    LEMON_TRACE_FN();

    uninitializeComponents();
    unregisterComponents();
}

void
Actor::onStartInternal()
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

    onStart();
}

void
Actor::onStopInternal()
{
    iterateSubObjects(
        [](GameObject* pObject) {
            if (auto* pComponent = game::cast<ActorComponent>(pObject)) {
                pComponent->onStop();
            }
        },
        true);

    onStop();
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
