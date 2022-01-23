#pragma once

#include <lemon/render/material/MaterialManager.h>
#include <lemon/render/ConstantBuffer.h>
#include <lemon/render/PipelineManager.h>
#include <lemon/render/RenderPass.h>
#include <lemon/render/DebugUI.h>
#include <lemon/scheduler/common.h>

namespace lemon::render {
    template<typename T>
    concept RenderFrameResource = std::default_initializable<T>;

    template<RenderFrameResource TResource>
    class RenderFrameResources {
        friend class RenderManager;

        std::array<TResource, kMaxRenderFramesInFlight> resources;

    public:
        RenderFrameResources() = default;

        RenderFrameResources(RenderFrameResources&&) = default;

        RenderFrameResources(const RenderFrameResources&) = default;

        RenderFrameResources&
        operator=(RenderFrameResources&&) = default;

        RenderFrameResources&
        operator=(const RenderFrameResources&) = default;

        const TResource&
        getResources(const RenderPassContext& ctx) const
        {
            return resources[ctx.frameIndex];
        }

        TResource&
        getResources(const RenderPassContext& ctx)
        {
            return resources[ctx.frameIndex];
        }

        const TResource&
        getResources(uint8_t index) const
        {
            LEMON_ASSERT(index < kMaxRenderFramesInFlight);
            return resources[index];
        }

        TResource&
        getResources(uint8_t index)
        {
            LEMON_ASSERT(index < kMaxRenderFramesInFlight);
            return resources[index];
        }
    };

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

        std::vector<wgpu::CommandBuffer> frameCommandBuffers;

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
        render();

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
