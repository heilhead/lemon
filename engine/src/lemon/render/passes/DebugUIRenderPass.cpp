#include <lemon/render/passes/DebugUIRenderPass.h>
#include <lemon/device/Device.h>
#include <lemon/game/actor/GameWorld.h>

using namespace lemon;
using namespace lemon::render;
using namespace lemon::device;
using namespace lemon::game;
using namespace lemon::scheduler;

DebugUIRenderPass::DebugUIRenderPass() : passDesc{}, colorAttachments{}
{
    passDesc.colorAttachmentCount = colorAttachments.size();
    passDesc.colorAttachments = colorAttachments.data();
    passDesc.depthStencilAttachment = nullptr;

    // Main color attachment.
    colorAttachments[0].loadOp = wgpu::LoadOp::Load;
    colorAttachments[0].storeOp = wgpu::StoreOp::Store;
}

VoidTask<RenderPassError>
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

    context.addCommandBuffer(encoder.Finish());

    co_return {};
}
