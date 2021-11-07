#pragma once

#include <lemon/render.h>

namespace lemon::render {
    enum class RenderPassError { Unknown };
    enum class FrameRenderError { Unknown };

    struct RendererResources {
        wgpu::TextureView backbufferView;
        wgpu::TextureView depthStencilView;
    };

    class RenderPass {
    public:
        virtual Task<wgpu::CommandBuffer, RenderPassError>
        execute(const RendererResources& resources) = 0;
    };

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

    class Renderer {
        // TODO: Adjust when more passes are introduced.
        static constexpr size_t kNumRenderPasses = 1;

        std::vector<std::unique_ptr<RenderPass>> passes;
        RendererResources resources;

    public:
        Renderer();

        VoidTask<FrameRenderError>
        render();
    };
} // namespace lemon::render
