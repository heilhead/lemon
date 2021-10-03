#pragma once

#include <lemon/render/ShaderCompiler.h>
#include <lemon/render/material/MaterialManager.h>
#include <dawn/webgpu_cpp.h>

namespace lemon::render {
    class RenderManager {
        wgpu::Device* pDevice;
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
            assert(pDevice != nullptr);
            return *pDevice;
        }

        // TODO: Remove this
        inline void
        setDevice(wgpu::Device& device)
        {
            pDevice = &device;
        }
    };
} // namespace lemon::render