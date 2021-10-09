#pragma once

#include <lemon/render/material/SurfaceMaterialInstance.h>
#include <lemon/render/material/ShaderCompiler.h>
#include <lemon/render/material/MaterialLayout.h>
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
        ShaderCompiler shaderCompiler;
        AtomicCache<ShaderProgram> shaderProgramCache{256};
        AtomicCache<MaterialLayout> materialLayoutCache{128};
        AtomicCache<wgpu::Texture> textureCache{128};

    public:
        MaterialManager();
        ~MaterialManager();

        inline ShaderCompiler&
        getShaderCompiler()
        {
            return shaderCompiler;
        }

        KeepAlive<ShaderProgram>
        getShader(const res::MaterialResource& material, const res::material::MaterialConfiguration& config);

        KeepAlive<MaterialLayout>
        getMaterialLayout(const res::MaterialResource& material, const ShaderProgram& program);

        KeepAlive<wgpu::Texture>
        getTexture(const res::TextureResource& texture);

        static MaterialManager*
        get();
    };
} // namespace lemon::render
