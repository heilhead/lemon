#pragma once

#include <dawn/webgpu_cpp.h>
#include <dawn/webgpu_cpp_print.h>
#include <dawn/dawn_proc.h>
#include <dawn_native/D3D12Backend.h>

namespace lemon::render {
    struct BindingLayoutEntryInitializationHelper : wgpu::BindGroupLayoutEntry {
        BindingLayoutEntryInitializationHelper(uint32_t entryBinding, wgpu::ShaderStage entryVisibility,
                                               wgpu::BufferBindingType bufferType,
                                               bool bufferHasDynamicOffset = false,
                                               uint64_t bufferMinBindingSize = 0)
        {
            binding = entryBinding;
            visibility = entryVisibility;
            buffer.type = bufferType;
            buffer.hasDynamicOffset = bufferHasDynamicOffset;
            buffer.minBindingSize = bufferMinBindingSize;
        }

        BindingLayoutEntryInitializationHelper(uint32_t entryBinding, wgpu::ShaderStage entryVisibility,
                                               wgpu::SamplerBindingType samplerType)
        {
            binding = entryBinding;
            visibility = entryVisibility;
            sampler.type = samplerType;
        }

        BindingLayoutEntryInitializationHelper(
            uint32_t entryBinding, wgpu::ShaderStage entryVisibility,
            wgpu::TextureSampleType textureSampleType,
            wgpu::TextureViewDimension textureViewDimension = wgpu::TextureViewDimension::e2D,
            bool textureMultisampled = false)
        {
            binding = entryBinding;
            visibility = entryVisibility;
            texture.sampleType = textureSampleType;
            texture.viewDimension = textureViewDimension;
            texture.multisampled = textureMultisampled;
        }

        BindingLayoutEntryInitializationHelper(
            uint32_t entryBinding, wgpu::ShaderStage entryVisibility,
            wgpu::StorageTextureAccess storageTextureAccess, wgpu::TextureFormat format,
            wgpu::TextureViewDimension textureViewDimension = wgpu::TextureViewDimension::e2D)
        {
            binding = entryBinding;
            visibility = entryVisibility;
            storageTexture.access = storageTextureAccess;
            storageTexture.format = format;
            storageTexture.viewDimension = textureViewDimension;
        }

        BindingLayoutEntryInitializationHelper(uint32_t entryBinding, wgpu::ShaderStage entryVisibility,
                                               wgpu::ExternalTextureBindingLayout* bindingLayout)
        {
            binding = entryBinding;
            visibility = entryVisibility;
            nextInChain = bindingLayout;
        }

        BindingLayoutEntryInitializationHelper(const wgpu::BindGroupLayoutEntry& entry)
            : wgpu::BindGroupLayoutEntry(entry)
        {
        }
    };
} // namespace lemon::render