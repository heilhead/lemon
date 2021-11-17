#include "DebugUIRenderPass.h"

#include <lemon/device/Device.h>
#include <lemon/game/actor/GameWorld.h>

using namespace lemon;
using namespace lemon::render;
using namespace lemon::device;
using namespace lemon::game;

DebugUIRenderPass::DebugUIRenderPass() : passDesc{}, colorAttachments{}
{
    passDesc.colorAttachmentCount = colorAttachments.size();
    passDesc.colorAttachments = colorAttachments.data();
    passDesc.depthStencilAttachment = nullptr;

    // Main color attachment.
    colorAttachments[0].loadOp = wgpu::LoadOp::Load;
    colorAttachments[0].storeOp = wgpu::StoreOp::Store;
}

Task<wgpu::CommandBuffer, RenderPassError>
DebugUIRenderPass::execute(const RenderPassContext& context)
{
    colorAttachments[0].view = context.pCurrentFrame->swapChainBackbufferView;

    auto* pRenderMan = RenderManager::get();

    wgpu::CommandEncoder encoder = pRenderMan->getDevice().CreateCommandEncoder();

    {
        wgpu::RenderPassEncoder pass = encoder.BeginRenderPass(&passDesc);

        pRenderMan->getDebugUI().render(pass);

        pass.EndPass();
    }

    co_return encoder.Finish();
}
