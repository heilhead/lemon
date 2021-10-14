#include <lemon/render/RenderManager.h>

using namespace lemon::render;

RenderManager::RenderManager() : pDevice{nullptr} {}

void
RenderManager::init(wgpu::Device& device)
{
    pDevice = &device;
    cbuffer.init(device);
    pipelineManager.init(device);
    materialManager.init(device);
}
