#pragma once

#include <lemon/render/common.h>
#include <lemon/render/RenderPass.h>

namespace lemon::render {
    class MainRenderPass : public RenderPass {
        static constexpr wgpu::Color kMainClearColor = {0.2f, 0.2f, 0.2f, 0.0f};

        wgpu::RenderPassDescriptor passDesc;
        std::array<wgpu::RenderPassColorAttachment, 1> colorAttachments;
        wgpu::RenderPassDepthStencilAttachment depthStencilAttachmentInfo;

    public:
        MainRenderPass();

        Task<wgpu::CommandBuffer, RenderPassError>
        execute(const RenderPassContext& context) override;
    };
} // namespace lemon::render
