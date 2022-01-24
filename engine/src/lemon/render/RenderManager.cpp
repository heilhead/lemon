#include <lemon/render/RenderManager.h>
#include <lemon/render/utils.h>
#include <lemon/device/Device.h>
#include <lemon/scheduler.h>

using namespace lemon::render;
using namespace lemon::device;
using namespace lemon::scheduler;

wgpu::TextureView
createColorTargetView(const wgpu::Device& device, uint32_t width, uint32_t height)
{
    wgpu::TextureDescriptor descriptor;
    descriptor.dimension = wgpu::TextureDimension::e2D;
    descriptor.size.width = width;
    descriptor.size.height = height;
    descriptor.size.depthOrArrayLayers = 1;
    descriptor.sampleCount = 1;
    descriptor.format = wgpu::TextureFormat::RGBA16Float;
    descriptor.mipLevelCount = 1;
    descriptor.usage = wgpu::TextureUsage::RenderAttachment | wgpu::TextureUsage::TextureBinding;
    auto tex = device.CreateTexture(&descriptor);
    return tex.CreateView();
}

RenderManager::RenderManager() : pDevice{nullptr}, passes{}, resources{} {}

RenderManager::~RenderManager() {}

void
RenderManager::init(wgpu::Device& device)
{
    pDevice = &device;
    cbuffer.init(device);
    pipelineManager.init(device);
    materialManager.init(device);

    auto [wndWidth, wndHeight] = Device::get()->getWindow()->getSize();

    renderTargetWidth = wndWidth;
    renderTargetHeight = wndHeight;

    for (auto& res : resources) {
        res.depthStencilView = createDefaultDepthStencilView(device, wndWidth, wndHeight);
        res.colorTargetView = createColorTargetView(device, wndWidth, wndHeight);
    }

    context.pCurrentFrame = &resources[0];
    context.pPreviousFrame = &resources[1];
}

void
RenderManager::releaseResources()
{
    debugUI.disable();

    for (auto& pass : passes) {
        pass->releaseResources();
    }

    passes.clear();

    for (auto& res : resources) {
        res.releaseResources();
    }

    materialManager.releaseResources();
    pipelineManager.releaseResources();
}

RenderPassResources&
RenderManager::getFrameResources(uint8_t inFrameIndex)
{
    LEMON_ASSERT(inFrameIndex < kMaxRenderFramesInFlight);
    return resources[inFrameIndex];
}

VoidTask<FrameRenderError>
RenderManager::renderFrame()
{
    OPTICK_EVENT();
    OPTICK_TAG("NumPasses", passes.size());

    auto* pGPU = Device::get()->getGPU();
    auto& swapChain = pGPU->getSwapChain();
    auto& queue = pGPU->getQueue();

    frameIndex = ++frameIndex % kMaxRenderFramesInFlight;
    cbuffer.reset();
    context.swap(frameIndex, &getFrameResources(frameIndex), swapChain.GetCurrentTextureView());

    pipelineManager.getSurfaceUniformData().merge(cbuffer);
    pipelineManager.getPostProcessUniformData().merge(cbuffer);

    for (auto& pass : passes) {
        OPTICK_EVENT("PreparePass");
        OPTICK_TAG("PassName", pass->getPassName());

        pass->prepare(context);
    }

    for (auto& pass : passes) {
        OPTICK_EVENT("ExecutePass");
        OPTICK_TAG("PassName", pass->getPassName());

        auto passError = co_await pass->execute(context);
        if (passError) {
            co_return FrameRenderError::Unknown;
        }
    }

    const auto& cmdBuf = context.getCommandBuffers();
    if (cmdBuf.size() > 0) {
        OPTICK_EVENT("SubmitCommandBuffers");
        OPTICK_TAG("NumCommandBuffers", cmdBuf.size());

        queue.Submit(cmdBuf.size(), cmdBuf.data());
    }

    {
        OPTICK_EVENT("Present");

        swapChain.Present();
    }

    frameRenderBaton.post();

    co_return {};
}

void
RenderManager::beginFrame()
{
    runRenderTask(renderFrame());
}

void
RenderManager::endFrame()
{
    frameRenderBaton.wait();
    frameRenderBaton.reset();
}
