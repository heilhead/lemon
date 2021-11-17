#include "PostProcessRenderPass.h"

#include <lemon/device/Device.h>
#include <lemon/game/actor/GameWorld.h>

using namespace lemon;
using namespace lemon::render;
using namespace lemon::device;

struct QuadVertex {
    glm::f32vec3 pos;
    glm::u16vec2 uv0;

    constexpr QuadVertex(glm::f32vec3 pos, glm::u16vec2 uv0) noexcept : pos{pos}, uv0{uv0} {}
};

static constexpr std::array<QuadVertex, 4> gQuadVertexData = {
    QuadVertex(glm::f32vec3(-1.f, -1.f, 0.f), math::fpack<uint16_t>(glm::f32vec2(0.f, 1.f))),
    QuadVertex(glm::f32vec3(1.f, -1.f, 0.f), math::fpack<uint16_t>(glm::f32vec2(1.f, 1.f))),
    QuadVertex(glm::f32vec3(1.f, 1.f, 0.f), math::fpack<uint16_t>(glm::f32vec2(1.f, 0.f))),
    QuadVertex(glm::f32vec3(-1.f, 1.f, 0.f), math::fpack<uint16_t>(glm::f32vec2(0.f, 0.f))),
};

static constexpr std::array<uint16_t, 6> gQuadIndexData = {0, 1, 3, 3, 1, 2};

MeshGPUBuffer
createQuadGPUBuffer()
{
    auto& device = RenderManager::get()->getDevice();
    MeshGPUBuffer data;

    data.vertexBuffer = createBufferFromData(device, gQuadVertexData.data(), sizeof(gQuadVertexData),
                                             wgpu::BufferUsage::Vertex);
    data.indexBuffer =
        createBufferFromData(device, gQuadIndexData.data(), sizeof(gQuadIndexData), wgpu::BufferUsage::Index);

    data.indexCount = gQuadIndexData.size();
    data.indexFormat = wgpu::IndexFormat::Uint16;

    return data;
}

PostProcessRenderPass::PostProcessRenderPass(PostProcessMaterialInstance material)
    : passDesc{}, colorAttachments{}, quadBuffer{createQuadGPUBuffer()}, material{material}
{
    passDesc.colorAttachmentCount = colorAttachments.size();
    passDesc.colorAttachments = colorAttachments.data();
    passDesc.depthStencilAttachment = nullptr;

    // Main color attachment.
    colorAttachments[0].loadOp = wgpu::LoadOp::Clear;
    colorAttachments[0].storeOp = wgpu::StoreOp::Store;
}

void
PostProcessRenderPass::prepare(const RenderPassContext& context)
{
    auto* pRenderMan = RenderManager::get();
    auto& cbuffer = pRenderMan->getConstantBuffer();
    material.getUniformData().merge(cbuffer);
}

Task<wgpu::CommandBuffer, RenderPassError>
PostProcessRenderPass::execute(const RenderPassContext& context)
{
    colorAttachments[0].view = context.pCurrentFrame->swapChainBackbufferView;

    auto* pRenderMan = RenderManager::get();
    auto* pPipelineMan = PipelineManager::get();

    auto& sharedData = pPipelineMan->getPostProcessUniformData();
    auto& materialData = material.getUniformData();

    wgpu::CommandEncoder encoder = pRenderMan->getDevice().CreateCommandEncoder();

    {
        wgpu::RenderPassEncoder pass = encoder.BeginRenderPass(&passDesc);

        pass.SetPipeline(material.getRenderPipeline().getMainPipeline());

        pass.SetBindGroup(kPostProcessSharedBindGroupIndex, context.pCurrentFrame->postProcessBindGroup,
                          sharedData.getOffsetCount(), sharedData.getOffsets());

        pass.SetBindGroup(kMaterialBindGroupIndex, material.getBindGroup(), materialData.getOffsetCount(),
                          materialData.getOffsets());

        pass.SetVertexBuffer(0, quadBuffer.vertexBuffer);
        pass.SetIndexBuffer(quadBuffer.indexBuffer, quadBuffer.indexFormat);
        pass.DrawIndexed(quadBuffer.indexCount);

        pass.EndPass();
    }

    co_return encoder.Finish();
}
