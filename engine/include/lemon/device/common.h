#pragma once

#include <GLFW/glfw3.h>

namespace lemon::device {
    using WindowHandle = GLFWwindow*;

    enum class LoopControl { Abort = 0, Continue = 1 };
} // namespace lemon::device
