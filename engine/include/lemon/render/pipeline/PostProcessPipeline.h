#pragma once

#include <lemon/render/pipeline/DynamicPipeline.h>

namespace lemon::render {
    struct DynamicMaterialSharedResources;

    class PostProcessPipeline : public DynamicPipeline {
        wgpu::RenderPipeline main;

    public:
        PostProcessPipeline(const DynamicMaterialSharedResources& matShared);

        inline const wgpu::RenderPipeline&
        getMainPipeline() const
        {
            return main;
        }
    };
} // namespace lemon::render
