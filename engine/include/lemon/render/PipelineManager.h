#pragma once

#include <lemon/render/material/MaterialConfiguration.h>
#include <lemon/render/material/MaterialUniformData.h>
#include <lemon/render/pipeline/SurfacePipeline.h>
#include <lemon/render/pipeline/PostProcessPipeline.h>

namespace lemon::render {
    struct SurfaceMaterialSharedResources;
    class MeshVertexFormat;
    struct MaterialLayout;
    class ShaderProgram;

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
        createPostProcessBindGroup(const wgpu::TextureView& colorTargetView);

        KeepAlive<PostProcessPipeline>
        getPostProcessPipeline(const PostProcessMaterialSharedResources& matShared);

    private:
        void
        initSurfaceBindGroup();

        void
        initPostProcessBindGroup();
    };
} // namespace lemon::render
