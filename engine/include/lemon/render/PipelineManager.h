#pragma once

#include <lemon/render/material/MaterialConfiguration.h>
#include <lemon/render/material/MaterialUniformData.h>

namespace lemon::render {
    class MaterialSharedResources;
    class MeshVertexFormat;
    struct MaterialLayout;
    class ShaderProgram;

    class MeshSurfacePipeline {
        struct PipelineConfiguration {
            const wgpu::PipelineLayoutDescriptor* pPipelineLayoutDesc;
            const wgpu::VertexBufferLayout* pVertexLayout;
            const wgpu::ShaderModule* pColorShaderModule;
            const wgpu::ShaderModule* pDepthShaderModule;
        };

        wgpu::RenderPipeline color;
        wgpu::RenderPipeline depth;

    public:
        MeshSurfacePipeline(const MaterialSharedResources& matShared, const MeshVertexFormat& vertexFormat);

        const wgpu::RenderPipeline&
        getColorPipeline() const
        {
            return color;
        }

        const wgpu::RenderPipeline&
        getDepthPipeline() const
        {
            return depth;
        }

    private:
        void
        createColorPipeline(const PipelineConfiguration& config);

        void
        createDepthPipeline(const PipelineConfiguration& config);
    };

    // TODO: Respec this to `GeometryPipelineManager` or `SurfacePipelineManager`.
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

        AtomicCache<MeshSurfacePipeline> pipelineCache{512};

    public:
        PipelineManager();

        void
        init(wgpu::Device& device);

        inline const MaterialConfiguration&
        getColorConfig()
        {
            return colorConfig;
        }

        inline const MaterialConfiguration&
        getDepthConfig()
        {
            return depthConfig;
        }

        inline const MaterialConfiguration&
        getPostProcessConfig()
        {
            return postProcessConfig;
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

        inline const KeepAlive<MaterialLayout>&
        getPostProcessBindGroupLayout() const
        {
            return kaPostProcessBindGroupLayout;
        }

        void
        assignPipelines(MaterialSharedResources& matShared, const MeshVertexFormat& vertexFormat);

        inline MaterialUniformData&
        getSurfaceUniformData()
        {
            return surfaceUniformData;
        }

        inline MaterialUniformData&
        getPostProcessUniformData()
        {
            return postProcessUniformData;
        }

        wgpu::BindGroup
        createPostProcessBindGroup(const wgpu::TextureView& colorTargetView);

    private:
        void
        initSurfaceBindGroup();

        void
        initPostProcessBindGroup();

        KeepAlive<MeshSurfacePipeline>
        getPipeline(const MaterialSharedResources& matShared, const MeshVertexFormat& vertexFormat);
    };
} // namespace lemon::render
