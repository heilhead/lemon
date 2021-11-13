#pragma once

namespace lemon::render {
    enum class RenderPassError { Unknown };
    enum class FrameRenderError { Unknown };

    struct RendererResources {
        wgpu::TextureView backbufferView;
        wgpu::TextureView depthStencilView;
    };
} // namespace lemon::render
