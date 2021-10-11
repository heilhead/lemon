#include <lemon/render/RenderManager.h>

using namespace lemon::render;

static RenderManager* gInstance;

RenderManager::RenderManager() : pDevice{nullptr}
{
    LEMON_ASSERT(gInstance == nullptr);
    gInstance = this;
}

RenderManager::~RenderManager()
{
    gInstance = nullptr;
}

RenderManager*
RenderManager::get()
{
    return gInstance;
}

void
RenderManager::init(wgpu::Device& device)
{
    pDevice = &device;
    cbuffer.init(device);
    pipelineManager.init(device);
    materialManager.init(device);
}