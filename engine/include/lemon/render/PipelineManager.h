#pragma once

#include <lemon/render/material/MaterialConfiguration.h>
#include <lemon/render/material/MaterialInstance.h>
#include <lemon/render/material/MaterialUniformData.h>
#include <lemon/render/pipeline/SurfacePipeline.h>
#include <lemon/render/pipeline/PostProcessPipeline.h>
#include <lemon/render/pipeline/DynamicPipeline.h>

namespace lemon::render {
    struct SurfaceMaterialSharedResources;
    class MeshVertexFormat;
    struct MaterialLayout;
    class ShaderProgram;

    template<class T>
    concept DynamicPipelineBase = Base<T, DynamicPipeline>;

    class PipelineManager : public UnsafeSingleton<PipelineManager> {
        enum class PipelineType { Color, Depth };

        wgpu::Device* pDevice;

        wgpu::BindGroup surfaceBindGroup;
        KeepAlive<MaterialLayout> kaSurfaceBindGroupLayout;
        MaterialUniformData surfaceUniformData;

        KeepAlive<MaterialLayout> kaPostProcessBindGroupLayout;
        MaterialUniformData postProcessUniformData;
        wgpu::Sampler postProcessColorTargetSampler;

        MaterialConfiguration colorConfig;
        MaterialConfiguration depthConfig;
        MaterialConfiguration postProcessConfig;

        AtomicCache<SurfacePipeline> surfacePipelineCache{512};
        AtomicCache<PostProcessPipeline> postProcessPipelineCache{512};
        AtomicCache<DynamicPipeline> dynamicPipelineCache{512};

    public:
        PipelineManager();

        void
        init(wgpu::Device& device);

        inline const MaterialConfiguration&
        getSurfaceColorConfig()
        {
            return colorConfig;
        }

        inline const MaterialConfiguration&
        getSurfaceDepthConfig()
        {
            return depthConfig;
        }

        inline const KeepAlive<MaterialLayout>&
        getSurfaceBindGroupLayout() const
        {
            return kaSurfaceBindGroupLayout;
        }

        inline const wgpu::BindGroup&
        getSurfaceBindGroup() const
        {
            return surfaceBindGroup;
        }

        KeepAlive<SurfacePipeline>
        getSurfacePipeline(const SurfaceMaterialSharedResources& matShared,
                           const MeshVertexFormat& vertexFormat);

        inline MaterialUniformData&
        getSurfaceUniformData()
        {
            return surfaceUniformData;
        }

        inline const MaterialConfiguration&
        getPostProcessConfig()
        {
            return postProcessConfig;
        }

        inline const KeepAlive<MaterialLayout>&
        getPostProcessBindGroupLayout() const
        {
            return kaPostProcessBindGroupLayout;
        }

        inline MaterialUniformData&
        getPostProcessUniformData()
        {
            return postProcessUniformData;
        }

        wgpu::BindGroup
        createPostProcessBindGroup(const wgpu::TextureView& colorTargetView) const;

        // KeepAlive<PostProcessPipeline>
        // getPostProcessPipeline(const PostProcessMaterialSharedResources& matShared);

        template<DynamicPipelineBase TDynamicPipeline>
        KeepAlive<DynamicPipeline>
        getDynamicPipeline(const DynamicMaterialSharedResources& matShared)
        {
            const auto id = lemon::hash(matShared.id, utils::getTypeID<TDynamicPipeline>());
            return dynamicPipelineCache.get(id, [&]() { return new TDynamicPipeline(matShared); });
        }

    private:
        void
        initSurfaceBindGroup();

        void
        initPostProcessBindGroup();
    };
} // namespace lemon::render
