#pragma once

namespace lemon::render {
    struct SurfaceMaterialSharedResources;
    class MeshVertexFormat;

    class SurfacePipeline {
        struct PipelineConfiguration {
            const wgpu::PipelineLayoutDescriptor* pPipelineLayoutDesc;
            const wgpu::VertexBufferLayout* pVertexLayout;
            const wgpu::ShaderModule* pColorShaderModule;
            const wgpu::ShaderModule* pDepthShaderModule;
        };

        wgpu::RenderPipeline color;
        wgpu::RenderPipeline depth;

    public:
        SurfacePipeline(const SurfaceMaterialSharedResources& matShared,
                        const MeshVertexFormat& vertexFormat);

        inline const wgpu::RenderPipeline&
        getColorPipeline() const
        {
            return color;
        }

        inline const wgpu::RenderPipeline&
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
} // namespace lemon::render
