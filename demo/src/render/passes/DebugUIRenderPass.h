#pragma once

#include "../common.h"
#include "../RenderPass.h"

namespace lemon::render {
    class DebugUIRenderPass : public RenderPass {
        wgpu::RenderPassDescriptor passDesc;
        std::array<wgpu::RenderPassColorAttachment, 1> colorAttachments;

        bool bShowDemoWindow = true;

    public:
        DebugUIRenderPass();

        Task<wgpu::CommandBuffer, RenderPassError>
        execute(const RendererResources& resources) override;
    };
} // namespace lemon::render
