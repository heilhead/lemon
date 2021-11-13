#pragma once

#include "../common.h"
#include "../RenderPass.h"

namespace lemon::render {
    class MainRenderPass : public RenderPass {
        static constexpr wgpu::Color kMainClearColor = {0.2f, 0.2f, 0.2f, 0.0f};

        wgpu::RenderPassDescriptor passDesc;
        std::array<wgpu::RenderPassColorAttachment, 1> colorAttachments;
        wgpu::RenderPassDepthStencilAttachment depthStencilAttachmentInfo;

    public:
        MainRenderPass();

        Task<wgpu::CommandBuffer, RenderPassError>
        execute(const RendererResources& resources) override;
    };
} // namespace lemon::render
