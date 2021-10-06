#pragma once

#include <lemon/render/material/MaterialManager.h>
#include <lemon/shared/assert.h>
#include <dawn/webgpu_cpp.h>

namespace lemon::render {
    class RenderManager {
        wgpu::Device* pDevice;
        MaterialManager materialManager;

    public:
        RenderManager();
        ~RenderManager();

        static RenderManager*
        get();

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

        // TODO: Remove this
        inline void
        setDevice(wgpu::Device& device)
        {
            pDevice = &device;
        }
    };
} // namespace lemon::render