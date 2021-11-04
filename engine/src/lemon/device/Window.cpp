#include <lemon/device/Window.h>
#include <GLFW/glfw3native.h>

using namespace lemon::device;

Window::Window(WindowDescriptor desc)
{
    if (!glfwInit()) {
        utils::halt("GLFW init failed");
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    window = glfwCreateWindow(desc.width, desc.height, "Lemon Engine", NULL, NULL);
    if (window == nullptr) {
        utils::halt("failed to create GLFW window");
    }

    hWnd = glfwGetWin32Window(window);
    width = desc.width;
    height = desc.height;
}

Window::~Window()
{
    // TODO: Should the window be destroyed here, or automatically from `glfwTerminate()` call? Destroying it
    // here causes `glfwTerminate()` to throw.
    // glfwDestroyWindow(window);
}

void
Window::loop(const std::function<LoopControl(float)>& callback) const
{
    typedef std::chrono::steady_clock clock;
    typedef std::chrono::duration<float> duration;

    auto tLastFrame = clock::now();
    while (!glfwWindowShouldClose(window)) {
        auto tNow = clock::now();
        duration dt = tNow - tLastFrame;
        auto ctrl = callback(dt.count());

        switch (ctrl) {
        case LoopControl::Continue:
            tLastFrame = tNow;
            break;
        case LoopControl::Abort:
            return;
        }
    }
}
