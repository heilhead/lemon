#pragma once

#include <lemon/render/material/SurfaceMaterialInstance.h>
#include <lemon/render/material/ShaderCompiler.h>
#include <lemon/resource/types/MaterialResource.h>
#include <lemon/shared/AtomicCache.h>

namespace lemon::render {
    class MaterialManager {
        static constexpr uint8_t kUserBindGroupIndex = 1;

        ShaderCompiler shaderCompiler;
        AtomicCache<ShaderProgram> shaderProgramCache{256};
        AtomicCache<wgpu::BindGroupLayout> bindGroupLayoutCache{128};

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
        getBindGroupLayout(const ShaderProgram& program);

        static MaterialManager*
        get();
    };
} // namespace lemon::render