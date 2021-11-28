#pragma once

namespace lemon::render {
    struct DynamicMaterialSharedResources;

    class DynamicPipeline {
    public:
        // DynamicPipeline(const DynamicMaterialSharedResources& matShared);
        ~DynamicPipeline() = default;

        virtual const wgpu::RenderPipeline&
        getMainPipeline() const = 0;
    };
} // namespace lemon::render
