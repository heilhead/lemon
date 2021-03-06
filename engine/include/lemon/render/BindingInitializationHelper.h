#pragma once

namespace lemon::render {
    struct BindingInitializationHelper {
        BindingInitializationHelper(uint32_t binding, const wgpu::Sampler& sampler)
            : binding(binding), sampler(sampler)
        {
        }

        BindingInitializationHelper(uint32_t binding, const wgpu::TextureView& textureView)
            : binding(binding), textureView(textureView)
        {
        }

        BindingInitializationHelper(uint32_t binding, const wgpu::ExternalTexture& externalTexture)
            : binding(binding)
        {
            externalTextureBindingEntry.externalTexture = externalTexture;
        }

        BindingInitializationHelper(uint32_t binding, const wgpu::Buffer& buffer, uint64_t offset = 0,
                                    uint64_t size = wgpu::kWholeSize)
            : binding(binding), buffer(buffer), offset(offset), size(size)
        {
        }

        wgpu::BindGroupEntry
        getAsBinding() const
        {
            wgpu::BindGroupEntry result;

            result.binding = binding;
            result.sampler = sampler;
            result.textureView = textureView;
            result.buffer = buffer;
            result.offset = offset;
            result.size = size;
            if (externalTextureBindingEntry.externalTexture != nullptr) {
                result.nextInChain = &externalTextureBindingEntry;
            }

            return result;
        }

        uint32_t binding;
        wgpu::Sampler sampler;
        wgpu::TextureView textureView;
        wgpu::Buffer buffer;
        wgpu::ExternalTextureBindingEntry externalTextureBindingEntry;
        uint64_t offset = 0;
        uint64_t size = 0;
    };

    template<typename... TArgs>
    wgpu::BindGroupEntry
    createBinding(TArgs&&... args)
    {
        return BindingInitializationHelper(std::forward<TArgs>(args)...).getAsBinding();
    }
} // namespace lemon::render
