#pragma once

#include <GLFW/glfw3.h>
#include <lemon/device/common.h>
#include <lemon/device/input/common.h>
#include <lemon/misc/Delegate.h>

namespace lemon::device {
    enum class CursorMode { Normal, Disabled, Raw };

    class MouseListener : public UnsafeSingleton<MouseListener> {
        std::unordered_map<MouseButton, MulticastDelegate<KeyEvent, KeyMod>> buttonListeners;
        MulticastDelegate<MouseButton, KeyEvent, KeyMod> globalListeners;
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
        getX() const
        {
            return x;
        }

        inline double
        getY() const
        {
            return y;
        }

        inline glm::f64vec2
        getPos() const
        {
            return glm::f64vec2(x, y);
        }

        MulticastDelegate<KeyEvent, KeyMod>&
        getDelegate(MouseButton btn);

        MulticastDelegate<MouseButton, KeyEvent, KeyMod>&
        getGlobalDelegate();

    private:
        void
        processButtonEvent(MouseButton btn, KeyEvent evt, KeyMod mods);

        static void
        mouseButtonCallback(WindowHandle handle, int btn, int evt, int mods);
    };
} // namespace lemon::device
