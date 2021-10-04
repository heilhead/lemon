#pragma once

#include <lemon/render/ShaderCompiler.h>
#include <lemon/render/material/MaterialManager.h>
#include <lemon/render/ResourceCache.h>
#include <lemon/shared/assert.h>
#include <dawn/webgpu_cpp.h>

namespace lemon::render {
    class RenderManager {
        wgpu::Device* pDevice;
        ResourceCache cache;
        ShaderCompiler shaderCompiler;
        MaterialManager materialManager;

    public:
        RenderManager();
        ~RenderManager();

        static RenderManager*
        get();

        inline ShaderCompiler&
        getShaderCompiler()
        {
            return shaderCompiler;
        }

        inline MaterialManager&
        getMaterialManager()
        {
            return materialManager;
        }

        inline wgpu::Device&
        getDevice()
        {
            LEMON_ASSERT(pDevice != nullptr);
            return *pDevice;
        }

        inline ResourceCache&
        getCache()
        {
            return cache;
        }

        // TODO: Remove this
        inline void
        setDevice(wgpu::Device& device)
        {
            pDevice = &device;
        }
    };
} // namespace lemon::render