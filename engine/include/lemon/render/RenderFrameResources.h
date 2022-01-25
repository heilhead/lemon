#pragma once

#include <lemon/render/common.h>

namespace lemon::render {
    template<typename T>
    concept RenderFrameResource = std::default_initializable<T>;

    template<RenderFrameResource TResource>
    class RenderFrameResources {
        friend class RenderManager;

        std::array<TResource, kMaxRenderFramesInFlight> resources;

    public:
        RenderFrameResources() = default;

        RenderFrameResources(RenderFrameResources&&) = default;

        RenderFrameResources(const RenderFrameResources&) = default;

        RenderFrameResources&
        operator=(RenderFrameResources&&) = default;

        RenderFrameResources&
        operator=(const RenderFrameResources&) = default;

        const TResource&
        getResources(const RenderPassContext& ctx) const
        {
            return resources[ctx.frameIndex];
        }

        TResource&
        getResources(const RenderPassContext& ctx)
        {
            return resources[ctx.frameIndex];
        }

        const TResource&
        getResources(uint8_t index) const
        {
            LEMON_ASSERT(index < kMaxRenderFramesInFlight);
            return resources[index];
        }

        TResource&
        getResources(uint8_t index)
        {
            LEMON_ASSERT(index < kMaxRenderFramesInFlight);
            return resources[index];
        }
    };
} // namespace lemon::render
