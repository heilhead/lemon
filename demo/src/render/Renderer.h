#pragma once

#include <lemon/render.h>
#include "common.h"
#include "RenderPass.h"

namespace lemon::render {
    class Renderer {
        // TODO: Adjust when more passes are introduced.
        static constexpr size_t kNumRenderPasses = 2;

        std::vector<std::unique_ptr<RenderPass>> passes;
        RenderPassResources resources;

    public:
        Renderer();

        VoidTask<FrameRenderError>
        render();
    };
} // namespace lemon::render
