#include "metadata.h"
// #include "bundles.h"
#include "mesh.h"

#include "game/actor/GameObject.h"
#include "game/actor/Actor.h"
#include "game/actor/ActorComponent.h"
#include "game/actor/GameObjectStore.h"

using namespace lemon;
using namespace lemon::game;

class MovementComponent : public ActorComponent {
public:
    MovementComponent()
    {
        LEMON_TRACE_FN();
    }

    ~MovementComponent()
    {
        LEMON_TRACE_FN();
    }

    void
    onRegister() override
    {
        LEMON_TRACE_FN();
    }

    void
    onInitialize() override
    {
        LEMON_TRACE_FN();
    }

    void
    onStart() override
    {
        LEMON_TRACE_FN();
    }

    void
    onTick(float deltaTime) override
    {
        LEMON_TRACE_FN();
    }

    void
    onStop() override
    {
        LEMON_TRACE_FN();
    }

    void
    onUninitialize() override
    {
        LEMON_TRACE_FN();
    }

    void
    onUnregister() override
    {
        LEMON_TRACE_FN();
    }
};

class CharacterActor : public Actor {
public:
    CharacterActor()
    {
        LEMON_TRACE_FN();

        auto* pMovementComponent = addComponent<MovementComponent>();

        LEMON_ASSERT(pMovementComponent != nullptr);
    }

    ~CharacterActor()
    {
        LEMON_TRACE_FN();
    }

    void
    onPreInitializeComponents() override
    {
        LEMON_TRACE_FN();
    }

    void
    onPostInitializeComponents() override
    {
        LEMON_TRACE_FN();
    }

    void
    onStart() override
    {
        LEMON_TRACE_FN();
    }

    void
    onTick(float deltaTime) override
    {
        LEMON_TRACE_FN();
    }

    void
    onStop() override
    {
        LEMON_TRACE_FN();
    }
};

int
main(int argc, char* argv[])
{

    // createMetadata();
    // testBundleLoading();
    // testMeshRendering();

    auto store = std::make_unique<GameObjectStore>();

    auto* pActor = store->create<CharacterActor>();

    pActor->onStartInternal();
    pActor->onTick(0.f);
    pActor->onStopInternal();

    // TODO: GameWorld, spawn/destroy actors, dispatch ticking

    return 0;
}
