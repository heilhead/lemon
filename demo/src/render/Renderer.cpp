#include "Renderer.h"
#include <lemon/device/Device.h>
#include <lemon/game/actor/GameWorld.h>

using namespace lemon;
using namespace lemon::render;
using namespace lemon::device;
using namespace lemon::game;

MainRenderPass::MainRenderPass() : passDesc{}, colorAttachments{}, depthStencilAttachmentInfo{}
{
    passDesc.colorAttachmentCount = colorAttachments.size();
    passDesc.colorAttachments = colorAttachments.data();
    passDesc.depthStencilAttachment = &depthStencilAttachmentInfo;

    // Main color attachment.
    colorAttachments[0].loadOp = wgpu::LoadOp::Clear;
    colorAttachments[0].storeOp = wgpu::StoreOp::Store;
    colorAttachments[0].clearColor = kMainClearColor;

    // Main depth attachment.
    depthStencilAttachmentInfo.clearDepth = 1.0f;
    depthStencilAttachmentInfo.clearStencil = 0;
    depthStencilAttachmentInfo.depthLoadOp = wgpu::LoadOp::Clear;
    depthStencilAttachmentInfo.depthStoreOp = wgpu::StoreOp::Store;
    depthStencilAttachmentInfo.stencilLoadOp = wgpu::LoadOp::Clear;
    depthStencilAttachmentInfo.stencilStoreOp = wgpu::StoreOp::Store;
}

Task<wgpu::CommandBuffer, RenderPassError>
MainRenderPass::execute(const RendererResources& resources)
{
    colorAttachments[0].view = resources.backbufferView;
    depthStencilAttachmentInfo.view = resources.depthStencilView;

    auto* pPipelineMan = PipelineManager::get();
    auto* pRenderMan = RenderManager::get();

    auto& device = pRenderMan->getDevice();
    auto& cbuffer = pRenderMan->getConstantBuffer();

    cbuffer.reset();

    auto& sharedData = pPipelineMan->getSharedUniformData();
    sharedData.merge(cbuffer);

    wgpu::CommandEncoder encoder = device.CreateCommandEncoder();

    {
        wgpu::RenderPassEncoder pass = encoder.BeginRenderPass(&passDesc);

        static constexpr auto matModel = lemon::sid("packetParams.matModel");

        for (auto& renderProxy : GameWorld::get()->getRenderQueue().getMeshes()) {
            renderProxy.pOwner->updateRenderProxy(renderProxy);

            auto& mat = renderProxy.material;
            auto& matData = mat.getUniformData();
            matData.setData(matModel, renderProxy.matrix);
            matData.merge(cbuffer);

            pass.SetPipeline(mat.getRenderPipeline().getColorPipeline());

            pass.SetBindGroup(kSharedBindGroupIndex, pPipelineMan->getSharedBindGroup(),
                              sharedData.getOffsetCount(), sharedData.getOffsets());

            pass.SetBindGroup(kMaterialBindGroupIndex, mat.getBindGroup(), matData.getOffsetCount(),
                              matData.getOffsets());

            pass.SetVertexBuffer(0, renderProxy.vertexBuffer);
            pass.SetIndexBuffer(renderProxy.indexBuffer, renderProxy.indexFormat);
            pass.DrawIndexed(renderProxy.indexCount);
        }

        pass.EndPass();
    }

    cbuffer.upload(device);

    co_return encoder.Finish();
}

Renderer::Renderer() : passes{}, resources{}
{
    auto* pDevice = Device::get();
    auto [wndWidth, wndHeight] = pDevice->getWindow()->getSize();

    resources.depthStencilView =
        createDefaultDepthStencilView(pDevice->getGPU()->getDevice(), wndWidth, wndHeight);

    passes.emplace_back(std::make_unique<MainRenderPass>());
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
