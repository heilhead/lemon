#pragma once

#include <GLFW/glfw3.h>
#include <lemon/device/common.h>
#include <lemon/device/input/common.h>

namespace lemon::device {
    class KeyboardListener {
        WindowHandle handle;

    public:
        KeyboardListener(WindowHandle handle);
    };
} // namespace lemon::input