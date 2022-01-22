#pragma once

#include <lemon/render/common.h>
#include <lemon/render/RenderPass.h>

namespace lemon::render {
    class BloomPipeline : public DynamicPipeline {
    public:
        wgpu::RenderPipeline main;
        wgpu::RenderPipeline prefilter;
        wgpu::RenderPipeline hblur;
        wgpu::RenderPipeline vblur;
        wgpu::RenderPipeline upsample;

    public:
        BloomPipeline(const DynamicMaterialSharedResources& matShared);

        const wgpu::RenderPipeline&
        getMainPipeline() const
        {
            return main;
        }
    };

    struct BloomUniformParams {
        float threshold;
        float strength;
        glm::f32vec2 texelSize;
        glm::f32vec4 lowTexSize; // xy texel size, zw width height
        float scatter;
        float clampMax;
        float thresholdKnee;
    };

    struct BloomMipPassData {
        float width;
        float height;
        wgpu::TextureView txDownsample;
        wgpu::TextureView txUpsample;
    };

    struct BloomStepData {
        wgpu::RenderPipeline pipeline;
        wgpu::BindGroup bindGroup;
        wgpu::TextureView target;
        glm::f32vec2 texelSize;
        glm::f32vec4 lowTexelSize;
        MaterialUniformData uniformData;
    };

    class PostProcessRenderPass : public RenderPass {
        static constexpr uint8_t kBloomMipLevels = 6;
        static constexpr uint8_t kBloomPasses = kBloomMipLevels - 1;

        wgpu::RenderPassDescriptor passDesc;
        std::array<wgpu::RenderPassColorAttachment, 1> colorAttachments;
        MeshGPUBuffer quadBuffer;
        DynamicMaterialInstance material;

        uint32_t bloomTargetWidth;
        uint32_t bloomTargetHeight;

        RenderFrameResources<wgpu::BindGroup> defaultBindGroup;
        MaterialUniformData bloomUniformData;
        BloomMipPassData bloomMips[kBloomMipLevels];
        wgpu::TextureView bloomPrefilterTarget;
        RenderFrameResources<BloomStepData> bloomPrefilterStep;
        RenderFrameResources<DynamicMaterialInstance> bloomMaterialResources;
        std::vector<BloomStepData> bloomSteps;

    public:
        PostProcessRenderPass(const res::MaterialResource* pPostProcessMaterial,
                              const res::MaterialResource* pBloomMaterial);

        virtual ~PostProcessRenderPass() override;

        template<std::semiregular TData>
        inline void
        setMaterialParameter(StringID id, const TData& val)
        {
            material.setParameter(id, val);
        }

        void
        prepare(const RenderPassContext& context) override;

        VoidTask<RenderPassError>
        execute(const RenderPassContext& context, std::vector<wgpu::CommandBuffer>& commandBuffers) override;
    };
} // namespace lemon::render
