#pragma once

#include <lemon/render/material/SurfaceMaterialInstance.h>
#include <lemon/render/material/ShaderCompiler.h>
#include <lemon/shared/AtomicCache.h>

namespace lemon::res {
    class MaterialResource;
    class TextureResource;

    namespace material {
        class MaterialConfiguration;
    }
} // namespace lemon::res

namespace lemon::render {
    class MaterialManager {
        static constexpr uint8_t kUserBindGroupIndex = 1;

        ShaderCompiler shaderCompiler;
        AtomicCache<ShaderProgram> shaderProgramCache{256};
        AtomicCache<wgpu::BindGroupLayout> bindGroupLayoutCache{128};
        AtomicCache<wgpu::Texture> textureCache{128};

    public:
        MaterialManager();
        ~MaterialManager();

        inline ShaderCompiler&
        getShaderCompiler()
        {
            return shaderCompiler;
        }

        ResourceRef<ShaderProgram>
        getShader(const res::MaterialResource& material, const res::material::MaterialConfiguration& config);

        ResourceRef<wgpu::BindGroupLayout>
        getBindGroupLayout(const res::MaterialResource& material, const ShaderProgram& program);

        ResourceRef<wgpu::Texture>
        getTexture(const res::TextureResource& texture);

        static MaterialManager*
        get();
    };
} // namespace lemon::render