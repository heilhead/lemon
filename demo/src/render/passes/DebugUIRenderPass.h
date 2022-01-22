#pragma once

#include <lemon/render/common.h>
#include <lemon/render/RenderPass.h>

namespace lemon::render {
    class DebugUIRenderPass : public RenderPass {
        wgpu::RenderPassDescriptor passDesc;
        std::array<wgpu::RenderPassColorAttachment, 1> colorAttachments;

        bool bShowDemoWindow = true;

    public:
        DebugUIRenderPass();

        VoidTask<RenderPassError>
        execute(const RenderPassContext& context, std::vector<wgpu::CommandBuffer>& commandBuffers) override;
    };
} // namespace lemon::render
