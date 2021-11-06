#include "DemoModelActor.h"

using namespace lemon;
using namespace lemon::game;
using namespace lemon::res;
using namespace demo;

DemoModelActor::DemoModelActor() : Actor()
{
    pMeshComponent = addComponent<StaticMeshComponent>();
    pMeshComponent->setModel(ResourceLocation("ozz-sample\\MannequinSkeleton.lem:SK_Mannequin"));
    pMeshComponent->setMaterial(0, ResourceLocation("misc\\M_Mannequin2"));

    root = pMeshComponent;
}

void
DemoModelActor::onStart()
{
    enableTick();
}

void
DemoModelActor::onTick(float dt)
{
    static constexpr float distance = 35.f;
    static constexpr float speed = 4.f;

    auto pos = getRoot()->getLocalPosition();
    pos.y = std::sin(GameWorld::get()->getTime() * speed + static_cast<float>(wave)) * distance;

    getRoot()->setLocalPosition(pos);
}
