#pragma once

#include <lemon/render/material/SurfaceMaterialInstance.h>
#include <lemon/render/material/ShaderCompiler.h>
#include <lemon/render/material/MaterialLayout.h>
#include <lemon/render/material/MaterialInstance.h>
#include <lemon/render/material/MaterialConfiguration.h>
#include <lemon/shared/AtomicCache.h>
#include <lemon/shared/UnsafeSingleton.h>

namespace lemon::res {
    class MaterialResource;
    class TextureResource;

    namespace material {
        class MaterialBlueprint;
        struct SamplerDescriptor;
    } // namespace material
} // namespace lemon::res

namespace lemon::render {
    class MaterialManager : public UnsafeSingleton<MaterialManager> {
        wgpu::Device* pDevice;
        ShaderCompiler shaderCompiler;
        AtomicCache<ShaderProgram> shaderProgramCache{256};
        AtomicCache<MaterialLayout> materialLayoutCache{128};
        AtomicCache<wgpu::Sampler> samplerCache{64};
        AtomicCache<wgpu::Texture> textureCache{128};
        AtomicCache<MaterialSharedResources> sharedResourcesCache{128};

    public:
        MaterialManager();

        inline ShaderCompiler&
        getShaderCompiler()
        {
            return shaderCompiler;
        }

        KeepAlive<ShaderProgram>
        getShader(const res::MaterialResource& material, const render::MaterialConfiguration& config);

        KeepAlive<ShaderProgram>
        getShader(const res::material::MaterialBlueprint& blueprint,
                  const render::MaterialConfiguration& config);

        KeepAlive<MaterialLayout>
        getMaterialLayout(const res::MaterialResource& material, const ShaderProgram& program,
                          uint8_t bindGroupIndex);

        KeepAlive<MaterialLayout>
        getMaterialLayout(const ShaderProgram& program, uint8_t bindGroupIndex);

        MaterialInstance
        getMaterialInstance(const res::MaterialResource& material, const MeshVertexFormat& vertexFormat);

        KeepAlive<wgpu::Sampler>
        getSampler(const res::material::SamplerDescriptor& desc);

        KeepAlive<wgpu::Sampler>
        getSampler(const wgpu::SamplerDescriptor& desc);

        KeepAlive<wgpu::Texture>
        getTexture(const res::TextureResource& texture);

        void
        init(wgpu::Device& device);

    private:
        wgpu::Texture
        createTexture(const res::TextureResource& textureRes);
    };
} // namespace lemon::render

template<>
struct folly::hasher<wgpu::SamplerDescriptor> {
    using folly_is_avalanching = std::true_type;

    size_t
    operator()(const wgpu::SamplerDescriptor& data) const
    {
        lemon::Hash hash;
        hash.append(data.addressModeU, data.addressModeV, data.addressModeW, data.magFilter, data.minFilter,
                    data.mipmapFilter, data.lodMinClamp, data.lodMaxClamp, data.compare, data.maxAnisotropy);
        return hash;
    }
};
