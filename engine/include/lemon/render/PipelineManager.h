#pragma once

#include <lemon/shared/logger.h>
#include <lemon/shared/AtomicCache.h>
#include <lemon/shared/UnsafeSingleton.h>
#include <lemon/render/material/MaterialConfiguration.h>
#include <lemon/render/material/MaterialUniformData.h>
#include <dawn/webgpu_cpp.h>

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

        wgpu::BindGroup sharedBindGroup;
        KeepAlive<MaterialLayout> kaSharedBindGroupLayout;
        MaterialUniformData sharedUniformData;

        MaterialConfiguration colorConfig;
        MaterialConfiguration depthConfig;
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

        inline const KeepAlive<MaterialLayout>&
        getSharedBindGroupLayout() const
        {
            return kaSharedBindGroupLayout;
        }

        inline const wgpu::BindGroup&
        getSharedBindGroup() const
        {
            return sharedBindGroup;
        }

        void
        assignPipelines(MaterialSharedResources& matShared, const MeshVertexFormat& vertexFormat);

        inline MaterialUniformData&
        getSharedUniformData()
        {
            return sharedUniformData;
        }

    private:
        void
        initSharedBindGroup();

        KeepAlive<MeshSurfacePipeline>
        getPipeline(const MaterialSharedResources& matShared, const MeshVertexFormat& vertexFormat);
    };
} // namespace lemon::render
