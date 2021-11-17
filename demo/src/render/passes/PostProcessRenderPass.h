#pragma once

#include <lemon/render/common.h>
#include <lemon/render/RenderPass.h>

namespace lemon::render {
    class PostProcessRenderPass : public RenderPass {
        wgpu::RenderPassDescriptor passDesc;
        std::array<wgpu::RenderPassColorAttachment, 1> colorAttachments;
        MeshGPUBuffer quadBuffer;
        PostProcessMaterialInstance material;

    public:
        PostProcessRenderPass(PostProcessMaterialInstance material);

        void
        prepare(const RenderPassContext& context) override;

        Task<wgpu::CommandBuffer, RenderPassError>
        execute(const RenderPassContext& context) override;
    };
} // namespace lemon::render
