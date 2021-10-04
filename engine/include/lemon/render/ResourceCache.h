#pragma once

#include <lemon/render/ShaderProgram.h>
#include <lemon/shared/AtomicCache.h>

namespace lemon::render {
    class ResourceCache {
        AtomicCache<ShaderProgram> shaderProgramCache{256};
        AtomicCache<wgpu::BindGroupLayout> bindGroupLayoutCache{128};

    public:
        ResourceCache();
        ~ResourceCache();

        static ResourceCache*
        get();

        static AtomicCache<ShaderProgram>&
        shaderProgram()
        {
            return get()->shaderProgramCache;
        }

        static AtomicCache<wgpu::BindGroupLayout>&
        bindGroupLayout()
        {
            return get()->bindGroupLayoutCache;
        }
    };
} // namespace lemon::render