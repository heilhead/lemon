#pragma once

namespace lemon::render {
    struct PostProcessMaterialSharedResources;

    class PostProcessPipeline {
        wgpu::RenderPipeline main;

    public:
        PostProcessPipeline(const PostProcessMaterialSharedResources& matShared);

        inline const wgpu::RenderPipeline&
        getMainPipeline() const
        {
            return main;
        }
    };
} // namespace lemon::render
