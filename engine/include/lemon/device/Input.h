#pragma once

#include <GLFW/glfw3.h>
#include <lemon/shared/UnsafeSingleton.h>
#include <lemon/device/common.h>
#include <lemon/device/input/KeyboardListener.h>
#include <lemon/device/input/MouseListener.h>

namespace lemon::device {
    class Input : public UnsafeSingleton<Input> {
        WindowHandle handle;
        KeyboardListener keyboardListener;
        MouseListener mouseListener;

    public:
        Input(WindowHandle inHandle);
    };
} // namespace lemon::device