#pragma once

#include <GLFW/glfw3.h>
#include <lemon/device/common.h>
#include <lemon/device/input/KeyboardListener.h>
#include <lemon/device/input/MouseListener.h>

namespace lemon {
    class Engine;
}

namespace lemon::device {
    class Input : public UnsafeSingleton<Input> {
        friend class Engine;

        WindowHandle handle;
        KeyboardListener keyboardListener;
        MouseListener mouseListener;

    public:
        Input(WindowHandle handle);

        KeyboardListener&
        getKeyboard()
        {
            return keyboardListener;
        }

        MouseListener&
        getMouse()
        {
            return mouseListener;
        }

    private:
        void
        update();
    };
} // namespace lemon::device
