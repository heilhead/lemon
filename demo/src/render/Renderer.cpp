#include "Renderer.h"
#include "passes/MainRenderPass.h"
#include "passes/DebugUIRenderPass.h"

#include <lemon/device/Device.h>
#include <lemon/game/actor/GameWorld.h>

using namespace lemon;
using namespace lemon::render;
using namespace lemon::device;
using namespace lemon::game;

Renderer::Renderer() : passes{}, resources{}
{
    auto* pDevice = Device::get();
    auto [wndWidth, wndHeight] = pDevice->getWindow()->getSize();

    resources.depthStencilView =
        createDefaultDepthStencilView(pDevice->getGPU()->getDevice(), wndWidth, wndHeight);

    passes.emplace_back(std::make_unique<MainRenderPass>());
    passes.emplace_back(std::make_unique<DebugUIRenderPass>());
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
