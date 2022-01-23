#pragma once

#include <lemon/render/material/ShaderCompiler.h>
#include <lemon/render/material/MaterialLayout.h>
#include <lemon/render/material/MaterialInstance.h>
#include <lemon/render/material/MaterialConfiguration.h>
#include <lemon/render/PipelineManager.h>

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
        AtomicCache<ShaderProgram> shaderProgramCache{512};
        AtomicCache<MaterialLayout> materialLayoutCache{512};
        AtomicCache<wgpu::Sampler> samplerCache{64};
        AtomicCache<wgpu::Texture> textureCache{512};
        AtomicCache<SurfaceMaterialSharedResources> surfaceSharedResourcesCache{512};
        AtomicCache<DynamicMaterialSharedResources> dynamicSharedResourcesCache{64};

    public:
        MaterialManager();

        void
        releaseResources();

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

        SurfaceMaterialInstance
        getSurfaceMaterialInstance(const res::MaterialResource& material,
                                   const MeshVertexFormat& vertexFormat);

        // PostProcessMaterialInstance
        // getPostProcessMaterialInstance(const res::MaterialResource& material);

        template<DynamicPipelineBase TDynamicPipeline>
        DynamicMaterialInstance
        getDynamicMaterialInstance(const res::MaterialResource& material,
                                   const DynamicMaterialResourceDescriptor& dynamicBindings,
                                   const MaterialConfiguration* pConfig = nullptr);

        KeepAlive<wgpu::Sampler>
        getSampler(const res::material::SamplerDescriptor& desc);

        KeepAlive<wgpu::Sampler>
        getSampler(const wgpu::SamplerDescriptor& desc);

        KeepAlive<wgpu::Texture>
        getTexture(const res::TextureResource& texture);

        void
        init(wgpu::Device& device);

        uint64_t
        calculateDynamicMaterialHash(const res::MaterialResource& material,
                                     const DynamicMaterialResourceDescriptor& dynamicBindings,
                                     const MaterialConfiguration* pConfig);

    private:
        wgpu::Texture
        createTexture(const res::TextureResource& textureRes);
    };

    template<DynamicPipelineBase TDynamicPipeline>
    DynamicMaterialInstance
    MaterialManager::getDynamicMaterialInstance(const res::MaterialResource& material,
                                                const DynamicMaterialResourceDescriptor& dynamicBindings,
                                                const MaterialConfiguration* pConfig)
    {
        const auto id = calculateDynamicMaterialHash(material, dynamicBindings, pConfig);
        const auto kaSharedResources = dynamicSharedResourcesCache.get(id, [&]() {
            auto* pMatShared = new DynamicMaterialSharedResources(id, material, dynamicBindings, pConfig);

            pMatShared->kaPipeline =
                PipelineManager::get()->getDynamicPipeline<TDynamicPipeline>(*pMatShared);

            return pMatShared;
        });

        return DynamicMaterialInstance(kaSharedResources);
    }
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
