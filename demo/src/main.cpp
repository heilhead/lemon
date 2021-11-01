#include "metadata.h"
// #include "bundles.h"
#include "mesh.h"

#include <lemon/game/actor/GameObject.h>
#include <lemon/game/actor/Actor.h>
#include <lemon/game/actor/ActorComponent.h>
#include <lemon/game/actor/GameObjectStore.h>
#include <lemon/game/actor/GameWorld.h>

#include "common/DemoModelActor.h"
#include "common/FlyingCameraActor.h"

using namespace lemon;
using namespace lemon::game;

class LocalMovementComponent : public ActorComponent {
public:
    LocalMovementComponent() : ActorComponent()
    {
        LEMON_TRACE_FN();
    }

    ~LocalMovementComponent()
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
        ActorComponent::onStop();

        LEMON_TRACE_FN();

        enableTick(1.f);
    }

    void
    onTick(float deltaTime) override
    {
        LEMON_TRACE_FN(" ", getParent()->getName());
    }

    void
    onStop() override
    {
        ActorComponent::onStop();

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

class MeshRenderComponent : public RenderableComponent {
    // render proxy?
    // draw call?

    // setMesh(mesh)
    // setMaterial(index, material)
};

class SkeletalMeshRenderComponent : public RenderableComponent {
};

class NonPositionableComponent : public ActorComponent {
};

class CharacterActor : public Actor {
public:
    CharacterActor() : Actor()
    {
        LEMON_TRACE_FN();

        root = addComponent<MeshRenderComponent>();

        auto* pMovementComponent = addComponent<LocalMovementComponent>();

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
        Actor::onStart();

        LEMON_TRACE_FN();

        enableTick();
    }

    void
    onTick(float deltaTime) override
    {
        LEMON_TRACE_FN(" ", getName());
    }

    void
    onStop() override
    {
        Actor::onStop();

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
    auto world = std::make_unique<GameWorld>();

    double time = 0.0f;

    auto hActor1 = world->createActor<CharacterActor>();
    hActor1.get()->setName("actor1");
    auto hActor2 = world->createActor<CharacterActor>();
    hActor2.get()->setName("actor2");

    auto* pActor1 = hActor1.get();
    auto* pActor2 = hActor2.get();

    pActor1->addTickDependency(pActor2);

    // world->updateInternal(time += 0.5f);
    // world->updateInternal(time += 0.5f);

    // auto* tickGroup = world->getComponentTickGroup();
    // const auto* p1 = tickGroup->getProxy(
    //     hActor1.get()->getComponent<MovementComponent>()->getTickDescriptor().getHandle());
    // const auto* p2 = tickGroup->getProxy(
    //     hActor2.get()->getComponent<MovementComponent>()->getTickDescriptor().getHandle());

    logger::trace("time: ", time);
    world->updateInternal(time += 0.5f);
    logger::trace("time: ", time);
    world->updateInternal(time += 0.5f);
    logger::trace("time: ", time);
    world->updateInternal(time += 0.5f);
    logger::trace("time: ", time);
    world->updateInternal(time += 0.5f);
    logger::trace("time: ", time);
    world->updateInternal(time += 0.5f);
    logger::trace("time: ", time);
    world->updateInternal(time += 0.5f);

    // LEMON_ASSERT(hActor);

    // auto* pActor = hActor.get();

    folly::small_vector<GameObject*, 4> comps;
    bool bFound = pActor1->findComponents<MeshRenderComponent>(comps);

    auto* pRenderable1 = pActor1->getComponent<SkeletalMeshRenderComponent>();

    world->destroyActor(hActor1);
    world->destroyActor(hActor2);

    LEMON_ASSERT(!hActor1);
    LEMON_ASSERT(!hActor2);

    // TODO: dispatch ticking

    return 0;
}
