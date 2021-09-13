#pragma once

#include <array>
#include <dawn/dawn_proc.h>
#include <dawn/webgpu_cpp.h>
#include <dawn_native/D3D12Backend.h>
#include <lemon/device/Window.h>

struct BindingLayoutEntryInitializationHelper : wgpu::BindGroupLayoutEntry {
    BindingLayoutEntryInitializationHelper(uint32_t entryBinding, wgpu::ShaderStage entryVisibility,
                                           wgpu::BufferBindingType bufferType,
                                           bool bufferHasDynamicOffset = false,
                                           uint64_t bufferMinBindingSize = 0);
    BindingLayoutEntryInitializationHelper(uint32_t entryBinding, wgpu::ShaderStage entryVisibility,
                                           wgpu::SamplerBindingType samplerType);
    BindingLayoutEntryInitializationHelper(
        uint32_t entryBinding, wgpu::ShaderStage entryVisibility, wgpu::TextureSampleType textureSampleType,
        wgpu::TextureViewDimension viewDimension = wgpu::TextureViewDimension::e2D,
        bool textureMultisampled = false);
    BindingLayoutEntryInitializationHelper(
        uint32_t entryBinding, wgpu::ShaderStage entryVisibility,
        wgpu::StorageTextureAccess storageTextureAccess, wgpu::TextureFormat format,
        wgpu::TextureViewDimension viewDimension = wgpu::TextureViewDimension::e2D);
    BindingLayoutEntryInitializationHelper(uint32_t entryBinding, wgpu::ShaderStage entryVisibility,
                                           wgpu::ExternalTextureBindingLayout* bindingLayout);

    explicit BindingLayoutEntryInitializationHelper(const wgpu::BindGroupLayoutEntry& entry);
};

struct BindingInitializationHelper {
    BindingInitializationHelper(uint32_t binding, const wgpu::Sampler& sampler);
    BindingInitializationHelper(uint32_t binding, const wgpu::TextureView& textureView);
    BindingInitializationHelper(uint32_t binding, const wgpu::ExternalTexture& externalTexture);
    BindingInitializationHelper(uint32_t binding, const wgpu::Buffer& buffer, uint64_t offset = 0,
                                uint64_t size = wgpu::kWholeSize);

    [[nodiscard]] wgpu::BindGroupEntry
    GetAsBinding() const;

    uint32_t binding;
    wgpu::Sampler sampler;
    wgpu::TextureView textureView;
    wgpu::Buffer buffer;
    wgpu::ExternalTextureBindingEntry externalTextureBindingEntry;
    uint64_t offset = 0;
    uint64_t size = 0;
};

static constexpr uint8_t kMaxVertexAttributes = 16u;
static constexpr uint8_t kMaxVertexBuffers = 8u;
static constexpr uint8_t kMaxColorAttachments = 8u;

class ComboRenderPipelineDescriptor : public wgpu::RenderPipelineDescriptor {
public:
    ComboRenderPipelineDescriptor();

    ComboRenderPipelineDescriptor(const ComboRenderPipelineDescriptor&) = delete;
    ComboRenderPipelineDescriptor&
    operator=(const ComboRenderPipelineDescriptor&) = delete;
    ComboRenderPipelineDescriptor(ComboRenderPipelineDescriptor&&) = delete;
    ComboRenderPipelineDescriptor&
    operator=(ComboRenderPipelineDescriptor&&) = delete;

    wgpu::DepthStencilState*
    EnableDepthStencil(wgpu::TextureFormat format = wgpu::TextureFormat::Depth24PlusStencil8);

    std::array<wgpu::VertexBufferLayout, kMaxVertexBuffers> cBuffers;
    std::array<wgpu::VertexAttribute, kMaxVertexAttributes> cAttributes;
    std::array<wgpu::ColorTargetState, kMaxColorAttachments> cTargets;
    std::array<wgpu::BlendState, kMaxColorAttachments> cBlends;

    wgpu::FragmentState cFragment;
    wgpu::DepthStencilState cDepthStencil;
};

struct ComboRenderPassDescriptor : public wgpu::RenderPassDescriptor {
public:
    ComboRenderPassDescriptor(std::initializer_list<wgpu::TextureView> colorAttachmentInfo,
                              wgpu::TextureView depthStencil = wgpu::TextureView());

    ComboRenderPassDescriptor(const ComboRenderPassDescriptor& otherRenderPass);
    ComboRenderPassDescriptor&
    operator=(const ComboRenderPassDescriptor& otherRenderPass);

    std::array<wgpu::RenderPassColorAttachmentDescriptor, kMaxColorAttachments> cColorAttachments;
    wgpu::RenderPassDepthStencilAttachmentDescriptor cDepthStencilAttachmentInfo = {};
};

class Triangle {
public:
    Triangle(const lemon::Window& window);

private:
    wgpu::Device device;
    wgpu::Buffer indexBuffer;
    wgpu::Buffer vertexBuffer;
    wgpu::Texture texture;
    wgpu::Sampler sampler;
    wgpu::Queue queue;
    wgpu::SwapChain swapChain;
    wgpu::TextureView depthStencilView;
    wgpu::RenderPipeline pipeline;
    wgpu::BindGroup bindGroup;

public:
    void
    initBuffers();
    void
    initTextures();
    void
    render();
};
