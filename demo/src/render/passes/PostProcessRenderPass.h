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

    class PostProcessRenderPass : public RenderPass {
        static constexpr uint8_t kBloomMipLevels = 7;

    public:
        struct BloomParams {
            float threshold;
            float scatter;
            float thresholdKnee;
            float clampMax;
            glm::f32vec4 srcTexSize;
            glm::f32vec4 lowTexSize;
        };

    private:
        struct BloomPassData {
            float width;
            float height;
            wgpu::TextureView txDownsample;
            wgpu::TextureView txUpsample;

            inline glm::f32vec4
            getTexSize() const
            {
                return glm::f32vec4(1.f / width, 1.f / height, width, height);
            }
        };

        struct BloomStepData {
            wgpu::RenderPipeline pipeline;
            wgpu::BindGroup bindGroup;
            wgpu::TextureView target;
            glm::f32vec4 srcTexSize;
            glm::f32vec4 lowTexSize;
            MaterialUniformData uniformData;
        };

        struct MainResources {
            MeshGPUBuffer quadBuffer;
            wgpu::RenderPassDescriptor passDesc;
            std::array<wgpu::RenderPassColorAttachment, 1> colorAttachments;
            DynamicMaterialInstance material;
            RenderFrameResources<wgpu::BindGroup> defaultBindGroup;

            MainResources(MeshGPUBuffer&& inQuadBuffer) : quadBuffer{inQuadBuffer} {}
        };

        struct BloomResources {
            MaterialUniformData bloomUniformData;
            BloomPassData bloomMips[kBloomMipLevels];
            wgpu::TextureView bloomPrefilterTarget;
            RenderFrameResources<BloomStepData> bloomPrefilterStep;
            RenderFrameResources<DynamicMaterialInstance> bloomMaterialResources;
            std::vector<BloomStepData> bloomSteps;
        };

        BloomParams bloomParams;

        std::unique_ptr<MainResources> mainResources;
        std::unique_ptr<BloomResources> bloomResources;

    public:
        PostProcessRenderPass(const res::MaterialResource* pPostProcessMaterial,
                              const res::MaterialResource* pBloomMaterial);

        virtual ~PostProcessRenderPass() override;

        template<std::semiregular TData>
        inline void
        setMaterialParameter(StringID id, const TData& val)
        {
            mainResources->material.setParameter(id, val);
        }

        virtual void
        releaseResources() override;

        virtual gsl::czstring<>
        getPassName() const override
        {
            return "PostProcess";
        }

        virtual void
        prepare(const RenderPassContext& context) override;

        virtual VoidTask<RenderPassError>
        execute(const RenderPassContext& context) override;

        BloomParams&
        getBloomParams()
        {
            return bloomParams;
        }

    private:
        void
        setBloomStepUniformData(BloomStepData& step);
    };
} // namespace lemon::render
