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

int
main(int argc, char* argv[])
{
    // createMetadata();
    // testBundleLoading();
    testMeshRendering();

    return 0;
}
