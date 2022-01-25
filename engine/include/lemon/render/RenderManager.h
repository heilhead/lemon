#pragma once

#include <lemon/render/material/MaterialManager.h>
#include <lemon/render/ConstantBuffer.h>
#include <lemon/render/PipelineManager.h>
#include <lemon/render/RenderPass.h>
#include <lemon/render/DebugUI.h>
#include <lemon/scheduler/common.h>
#include <lemon/render/RenderFrameResources.h>

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
        std::array<RenderPassResources, kMaxRenderFramesInFlight> resources;
        RenderPassContext context;
        uint8_t frameIndex{0};

        uint32_t renderTargetWidth;
        uint32_t renderTargetHeight;

        folly::Baton<> frameRenderBaton;

    public:
        template<typename TResource>
        using CustomResourceInitFn = std::function<TResource(const RenderPassResources&, uint8_t)>;

        RenderManager();

        ~RenderManager();

        void
        init(wgpu::Device& device);

        void
        releaseResources();

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

        template<Base<RenderPass> TRenderPass, typename... TArgs>
        inline TRenderPass*
        addRenderPass(TArgs&&... args)
        {
            passes.emplace_back(std::make_unique<TRenderPass>(std::forward<TArgs>(args)...));
            return static_cast<TRenderPass*>(passes.back().get());
        }

        template<Base<RenderPass> TRenderPass>
        inline TRenderPass*
        getRenderPass()
        {
            for (const auto& pass : passes) {
                if (auto* pPass = dynamic_cast<TRenderPass*>(pass.get())) {
                    return pPass;
                }
            }

            return nullptr;
        }

        inline uint8_t
        getFrameIndex()
        {
            return frameIndex;
        }

        inline std::pair<uint32_t, uint32_t>
        getRenderTargetSize()
        {
            return {renderTargetWidth, renderTargetHeight};
        }

        RenderPassResources&
        getFrameResources(uint8_t inFrameIndex);

        scheduler::VoidTask<FrameRenderError>
        renderFrame();

        void
        beginFrame();

        void
        endFrame();

        template<RenderFrameResource TResource>
        RenderFrameResources<TResource>
        createFrameResources(const CustomResourceInitFn<TResource>& initFn)
        {
            RenderFrameResources<TResource> result;

            for (uint8_t i = 0; i < kMaxRenderFramesInFlight; i++) {
                result.resources[i] = initFn(resources[i], i);
            }

            return result;
        }

        template<RenderFrameResource TResource>
        void
        createFrameResources(RenderFrameResources<TResource>& inResources,
                             const CustomResourceInitFn<TResource>& initFn)
        {
            for (uint8_t i = 0; i < kMaxRenderFramesInFlight; i++) {
                inResources.resources[i] = initFn(resources[i], i);
            }
        }
    };
} // namespace lemon::render
