#pragma once

#include <dawn/webgpu_cpp.h>
#include <dawn/webgpu_cpp_print.h>
#include <dawn/dawn_proc.h>
#include <dawn_native/D3D12Backend.h>
#include <lemon/render/BindingInitializationHelper.h>
#include <lemon/render/BindingLayoutEntryInitializationHelper.h>

namespace lemon::render {
    wgpu::TextureDataLayout
    createTextureDataLayout(uint64_t offset, uint32_t bytesPerRow, uint32_t rowsPerImage);

    wgpu::Buffer
    createBufferFromData(const wgpu::Device& device, const void* data, uint64_t size,
                         wgpu::BufferUsage usage);

    wgpu::ImageCopyBuffer
    createImageCopyBuffer(wgpu::Buffer buffer, uint64_t offset, uint32_t bytesPerRow,
                          uint32_t rowsPerImage = wgpu::kCopyStrideUndefined);

    wgpu::ImageCopyTexture
    createImageCopyTexture(wgpu::Texture texture, uint32_t mipLevel, wgpu::Origin3D origin,
                           wgpu::TextureAspect aspect = wgpu::TextureAspect::All);

    wgpu::ShaderModule
    createShaderModule(const wgpu::Device& device, const char* source);

    wgpu::BindGroupLayout
    makeBindGroupLayout(const wgpu::Device& device,
                        std::initializer_list<BindingLayoutEntryInitializationHelper> entriesInitializer);

    wgpu::PipelineLayout
    makeBasicPipelineLayout(const wgpu::Device& device, const wgpu::BindGroupLayout* bindGroupLayout);

    wgpu::TextureView
    createDefaultDepthStencilView(const wgpu::Device& device, uint32_t width, uint32_t height);

    wgpu::BindGroup
    makeBindGroup(const wgpu::Device& device, const wgpu::BindGroupLayout& layout,
                  std::initializer_list<BindingInitializationHelper> entriesInitializer);
} // namespace lemon::render
