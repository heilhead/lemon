#pragma once

#include <lemon/render/common.h>
#include <lemon/render/RenderPass.h>

namespace lemon::render {
    class DebugUIRenderPass : public RenderPass {
        wgpu::RenderPassDescriptor passDesc;
        std::array<wgpu::RenderPassColorAttachment, 1> colorAttachments;

        bool bShowDemoWindow = true;

    public:
        DebugUIRenderPass();

        virtual VoidTask<RenderPassError>
        execute(const RenderPassContext& context) override;

        virtual gsl::czstring<>
        getPassName() const override
        {
            return "DebugUI";
        }
    };
} // namespace lemon::render
