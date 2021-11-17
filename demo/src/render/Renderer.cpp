#include "Renderer.h"

#include <lemon/device/Device.h>

using namespace lemon;
using namespace lemon::render;
using namespace lemon::device;

wgpu::TextureView
createHDRBackbuffer(const wgpu::Device& device, uint32_t width, uint32_t height)
{
    wgpu::TextureDescriptor descriptor;
    descriptor.dimension = wgpu::TextureDimension::e2D;
    descriptor.size.width = width;
    descriptor.size.height = height;
    descriptor.size.depthOrArrayLayers = 1;
    descriptor.sampleCount = 1;
    descriptor.format = wgpu::TextureFormat::RGBA16Float;
    descriptor.mipLevelCount = 1;
    descriptor.usage = wgpu::TextureUsage::RenderAttachment;
    auto tex = device.CreateTexture(&descriptor);
    return tex.CreateView();
}

Renderer::Renderer() : passes{}, resources{}
{
    auto* pDevice = Device::get();
    auto [wndWidth, wndHeight] = pDevice->getWindow()->getSize();

    auto& device = pDevice->getGPU()->getDevice();

    resources.depthStencilView = createDefaultDepthStencilView(device, wndWidth, wndHeight);
    resources.backbufferHDRView = createHDRBackbuffer(device, wndWidth, wndHeight);

    // passes.emplace_back(std::make_unique<MainRenderPass>());
    // passes.emplace_back(std::make_unique<ColorCorrectionRenderPass>());
    // passes.emplace_back(std::make_unique<DebugUIRenderPass>());
}

VoidTask<FrameRenderError>
Renderer::render()
{
    auto* pGPU = Device::get()->getGPU();
    auto& swapChain = pGPU->getSwapChain();
    auto& queue = pGPU->getQueue();

    resources.backbufferView = swapChain.GetCurrentTextureView();

    folly::small_vector<wgpu::CommandBuffer, kNumRenderPasses> commands;

    for (auto& pass : passes) {
        auto passResult = co_await pass->execute(resources);
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
