#pragma once

#include "common.h"

namespace lemon::render {
    class RenderPass {
    public:
        virtual Task<wgpu::CommandBuffer, RenderPassError>
        execute(const RendererResources& resources) = 0;
    };
} // namespace lemon::render
