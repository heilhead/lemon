#include <iostream>
#include <mutex>

#include <folly/experimental/coro/BlockingWait.h>
#include <folly/experimental/coro/Collect.h>
#include <folly/PackedSyncPtr.h>
#include <lemon/scheduler.h>

#include <lemon/resource/ResourceManager.h>
#include <lemon/shared.h>
#include <lemon/shared/filesystem.h>

#include <lemon/serialization.h>

#include <folly/PackedSyncPtr.h>
#include <lemon/resource/ResourceContract.h>
#include <lemon/resource/ResourceLocation.h>
#include <lemon/resource/types/MaterialResource.h>
#include <lemon/resource/types/TextureResource.h>
#include <lemon/resource/types/BundleResource.h>

#include <dawn/webgpu_cpp.h>

#include "metadata.h"
#include "bundles.h"
#include "mesh.h"
#include "shader.h"

using namespace lemon::scheduler;
using namespace lemon::res;
using namespace lemon;

int
main(int argc, char* argv[])
{
    // createMetadata();
    // testBundleLoading();
    // testMeshLoading();
    testMeshRendering();
    // testShader();

    return 0;
}
