#pragma once

#include <lemon/pch/imgui.h>

namespace lemon::render {
    class DebugUI : public UnsafeSingleton<DebugUI> {
        bool bEnabled = false;

    public:
        bool
        isEnabled() const;

        void
        enable();

        void
        disable();

        void
        update() const;

        void
        render(const wgpu::RenderPassEncoder& pass) const;
    };
} // namespace lemon::render
