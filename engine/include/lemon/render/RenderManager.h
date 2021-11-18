#pragma once

#include <lemon/render/material/MaterialManager.h>
#include <lemon/render/ConstantBuffer.h>
#include <lemon/render/PipelineManager.h>
#include <lemon/render/RenderPass.h>
#include <lemon/render/DebugUI.h>
#include <lemon/scheduler/common.h>

namespace lemon::render {
    class RenderManager : public UnsafeSingleton<RenderManager> {
        // Adjust when more passes are introduced.
        static constexpr size_t kNumRenderPasses = 8;

        wgpu::Device* pDevice;
        ConstantBuffer cbuffer;
        MaterialManager materialManager;
        PipelineManager pipelineManager;
        DebugUI debugUI;

        std::vector<std::unique_ptr<RenderPass>> passes;
        std::array<RenderPassResources, 2> resources;
        RenderPassContext context;

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

        inline DebugUI&
        getDebugUI()
        {
            return debugUI;
        }

        template<Base<RenderPass> TRenderPass = RenderPass>
        inline TRenderPass*
        addRenderPass(std::unique_ptr<RenderPass> pass)
        {
            passes.emplace_back(std::move(pass));
            return dynamic_cast<TRenderPass*>(passes.back().get());
        }

        scheduler::VoidTask<FrameRenderError>
        render();
    };
} // namespace lemon::render
