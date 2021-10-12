#pragma once

#include <GLFW/glfw3.h>
#include <cstdint>
#include <functional>
#include <windows.h>

namespace lemon::device {
    enum class WindowKind { Window, Fullscreen, WindowedFullscreen };

    enum class LoopControl { Continue, Abort };

    struct WindowDescriptor {
        WindowKind kind{WindowKind::Window};
        uint32_t width{1280};
        uint32_t height{800};
    };

    class Window {
    public:
        explicit Window(WindowDescriptor desc);
        ~Window();

    protected:
        HWND hWnd;
        GLFWwindow* window;
        uint32_t width;
        uint32_t height;

    public:
        [[nodiscard]] HWND
        getContextHandle() const
        {
            return hWnd;
        }

        [[nodiscard]] std::pair<uint32_t, uint32_t>
        getSize() const
        {
            return {width, height};
        }

        void
        loop(const std::function<LoopControl(float)>& callback) const;
    };
} // namespace lemon::device
