#include "Window.h"

#define GLFW_EXPOSE_NATIVE_WIN32

#include <chrono>
#include <GLFW/glfw3native.h>
#include <lemon/prelude.h>

using namespace lemon;

Window::Window(WindowDescriptor& desc)
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

Window::~Window() {
    glfwDestroyWindow(window);
}

void Window::loop(const std::function<LoopControl(float)>& callback) const
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
            glfwPollEvents();
            break;
        case LoopControl::Abort:
            return;
        }
    }
}
