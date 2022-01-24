#pragma once

#include <lemon/render/common.h>
#include <lemon/scheduler/common.h>

namespace lemon::render {
    class RenderPass {
    public:
        virtual ~RenderPass() = default;

        virtual void
        releaseResources()
        {
        }

        virtual gsl::czstring<>
        getPassName() const
        {
            return "Unknown";
        }

        virtual void
        prepare(const RenderPassContext& context)
        {
        }

        virtual scheduler::VoidTask<RenderPassError>
        execute(const RenderPassContext& context) = 0;
    };
} // namespace lemon::render
