#pragma once

#include <lemon/shared/logger.h>
#include <lemon/shared/AtomicCache.h>
#include <lemon/render/material/MaterialConfiguration.h>
#include <dawn/webgpu_cpp.h>

namespace lemon::render {
    class PipelineManager {
        wgpu::Device* pDevice;
        wgpu::BindGroupLayout surfaceSharedBGLayout;
        wgpu::BindGroup surfaceSharedBG;
        MaterialConfiguration surfaceMainConfig;
        MaterialConfiguration surfaceDepthConfig;
        AtomicCache<wgpu::RenderPipeline> pipelineCache{512};

    public:
        PipelineManager();
        ~PipelineManager();

        static PipelineManager*
        get();

        void
        init(wgpu::Device& device);

        inline const MaterialConfiguration&
        getSurfaceMainConfig()
        {
            return surfaceMainConfig;
        }

        inline const MaterialConfiguration&
        getSurfaceDepthConfig()
        {
            return surfaceDepthConfig;
        }
    };
} // namespace lemon::render
