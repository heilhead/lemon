#pragma once

#include <lemon/render/common.h>
#include <lemon/scheduler/common.h>

namespace lemon::render {
    class RenderPass {
    public:
        virtual void
        prepare(const RenderPassContext& context)
        {
        }

        virtual scheduler::Task<wgpu::CommandBuffer, RenderPassError>
        execute(const RenderPassContext& context) = 0;
    };
} // namespace lemon::render
