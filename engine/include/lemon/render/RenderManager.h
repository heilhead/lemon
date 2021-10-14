#pragma once

#include <lemon/render/material/MaterialManager.h>
#include <lemon/render/ConstantBuffer.h>
#include <lemon/render/PipelineManager.h>
#include <lemon/shared/logger.h>
#include <lemon/shared/UnsafeSingleton.h>
#include <dawn/webgpu_cpp.h>

namespace lemon::render {
    class RenderManager : public UnsafeSingleton<RenderManager> {
        wgpu::Device* pDevice;
        ConstantBuffer cbuffer;
        MaterialManager materialManager;
        PipelineManager pipelineManager;

    public:
        RenderManager();

        void
        init(wgpu::Device& device);

        inline MaterialManager&
        getMaterialManager()
        {
            return materialManager;
        }

        inline PipelineManager&
        getPipelineManager()
        {
            return pipelineManager;
        }

        inline ConstantBuffer&
        getConstantBuffer()
        {
            return cbuffer;
        }

        inline wgpu::Device&
        getDevice()
        {
            LEMON_ASSERT(pDevice != nullptr);
            return *pDevice;
        }
    };
} // namespace lemon::render
