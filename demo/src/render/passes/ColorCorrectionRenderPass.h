#pragma once

#include <lemon/render/common.h>
#include <lemon/render/RenderPass.h>

namespace lemon::render {
    class ColorCorrectionRenderPass : public RenderPass {
        wgpu::RenderPassDescriptor passDesc;
        std::array<wgpu::RenderPassColorAttachment, 1> colorAttachments;

        bool bShowDemoWindow = true;

    public:
        ColorCorrectionRenderPass();

        Task<wgpu::CommandBuffer, RenderPassError>
        execute(const RenderPassResources& resources) override;
    };
} // namespace lemon::render
