#pragma once

#include <GLFW/glfw3.h>
#include <lemon/device/common.h>
#include <lemon/device/input/common.h>

namespace lemon::device {
    enum class CursorMode { Normal, Disabled };

    class MouseListener : public UnsafeSingleton<MouseListener> {
        WindowHandle handle;
        CursorMode mode{CursorMode::Normal};
        double x{0.f};
        double y{0.f};

    public:
        MouseListener(WindowHandle handle);

        void
        update();

        void
        setCursorMode(CursorMode mode);

        inline double
        getX()
        {
            return x;
        }

        inline double
        getY()
        {
            return y;
        }

        inline glm::f64vec2
        getPos()
        {
            return glm::f64vec2(x, y);
        }

    private:
        void
        processButtonEvent(MouseButton btn, KeyEvent evt, KeyMod mods);

        static void
        mouseButtonCallback(WindowHandle handle, int btn, int evt, int mods);
    };
} // namespace lemon::device
