#include <lemon/device/Input.h>

using namespace lemon::device;

Input::Input(WindowHandle inHandle) : keyboardListener{inHandle}, mouseListener{inHandle}
{
    handle = inHandle;
}

void
Input::update()
{
    OPTICK_EVENT()

    glfwPollEvents();
    mouseListener.update();
}
