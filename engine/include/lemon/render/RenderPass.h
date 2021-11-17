#pragma once

#include <lemon/render/common.h>
#include <lemon/scheduler/common.h>

namespace lemon::render {
    class RenderPass {
    public:
        virtual scheduler::Task<wgpu::CommandBuffer, RenderPassError>
        execute(const RenderPassResources& resources) = 0;
    };
} // namespace lemon::render
