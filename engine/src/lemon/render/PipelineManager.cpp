#include <lemon/render/PipelineManager.h>
#include <lemon/shared/logger.h>

using namespace lemon;
using namespace lemon::render;
// using namespace lemon::res;

static PipelineManager* gInstance;

PipelineManager::PipelineManager() : pDevice{nullptr}
{
    LEMON_ASSERT(gInstance == nullptr);
    gInstance = this;

    surfaceMainConfig.define("PIPELINE_DEPTH_ONLY", false);

    surfaceDepthConfig = surfaceMainConfig;
    surfaceDepthConfig.define("PIPELINE_DEPTH_ONLY", true);
}

PipelineManager::~PipelineManager()
{
    gInstance = nullptr;
}

PipelineManager*
PipelineManager::get()
{
    return gInstance;
}

void
PipelineManager::init(wgpu::Device& device)
{
    pDevice = &device;

    // create shared bind group layout
    // create shared bind group
}
