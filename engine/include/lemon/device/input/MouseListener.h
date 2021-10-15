#pragma once

#include <GLFW/glfw3.h>
#include <lemon/device/common.h>
#include <lemon/device/input/common.h>

namespace lemon::device {
    class MouseListener {
        WindowHandle handle;

    public:
        MouseListener(WindowHandle handle);
    };
} // namespace lemon::input