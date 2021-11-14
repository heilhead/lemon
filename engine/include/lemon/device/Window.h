#pragma once

#include <GLFW/glfw3.h>
#include <lemon/device/common.h>

namespace lemon::device {
    enum class WindowKind { Window, Fullscreen, WindowedFullscreen };

    enum class LoopControl { Abort = 0, Continue = 1 };

    struct WindowDescriptor {
        WindowKind kind{WindowKind::Window};
        uint32_t width{1280};
        uint32_t height{800};
    };

    class Window {
        HWND hWnd;
        WindowHandle window;
        uint32_t width;
        uint32_t height;

    public:
        explicit Window(WindowDescriptor desc);
        ~Window();

        inline HWND
        getContextHandle() const
        {
            return hWnd;
        }

        inline std::pair<uint32_t, uint32_t>
        getSize() const
        {
            return {width, height};
        }

        void
        loop(const std::function<LoopControl(float)>& callback) const;

        inline WindowHandle
        getHandle()
        {
            return window;
        }
    };
} // namespace lemon::device
