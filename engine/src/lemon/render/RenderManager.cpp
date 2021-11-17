#include <lemon/render/RenderManager.h>
#include <lemon/render/utils.h>
#include <lemon/device/Device.h>

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

void
RenderManager::init(wgpu::Device& device)
{
    pDevice = &device;
    cbuffer.init(device);
    pipelineManager.init(device);
    materialManager.init(device);

    auto [wndWidth, wndHeight] = Device::get()->getWindow()->getSize();

    for (auto& res : resources) {
        res.depthStencilView = createDefaultDepthStencilView(device, wndWidth, wndHeight);
        res.colorTargetView = createColorTargetView(device, wndWidth, wndHeight);
        res.postProcessBindGroup = pipelineManager.createPostProcessBindGroup(res.colorTargetView);
    }

    context.pCurrentFrame = &resources[0];
    context.pPreviousFrame = &resources[1];
}

void
RenderManager::addRenderPass(std::unique_ptr<RenderPass> pass)
{
    passes.emplace_back(std::move(pass));
}

VoidTask<FrameRenderError>
RenderManager::render()
{
    auto* pGPU = Device::get()->getGPU();
    auto& swapChain = pGPU->getSwapChain();
    auto& queue = pGPU->getQueue();

    cbuffer.reset();
    context.swap(swapChain.GetCurrentTextureView());

    pipelineManager.getSurfaceUniformData().merge(cbuffer);
    pipelineManager.getPostProcessUniformData().merge(cbuffer);

    for (auto& pass : passes) {
        pass->prepare(context);
    }

    folly::small_vector<wgpu::CommandBuffer, kNumRenderPasses> commands;

    for (auto& pass : passes) {
        auto passResult = co_await pass->execute(context);
        if (passResult) {
            commands.emplace_back(*passResult);
        } else {
            co_return FrameRenderError::Unknown;
        }
    }

    queue.Submit(commands.size(), commands.data());

    swapChain.Present();

    co_return {};
}
