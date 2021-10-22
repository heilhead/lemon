#include <lemon/device/input/MouseListener.h>

using namespace lemon;
using namespace lemon::device;

MouseListener::MouseListener(WindowHandle inHandle)
{
    handle = inHandle;
    glfwSetMouseButtonCallback(handle, mouseButtonCallback);
}

void
MouseListener::update()
{
    glfwGetCursorPos(handle, &x, &y);
}

void
MouseListener::setCursorMode(CursorMode inMode)
{
    switch (inMode) {
    case lemon::device::CursorMode::Normal:
        glfwSetInputMode(handle, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        break;
    case lemon::device::CursorMode::Disabled:
        glfwSetInputMode(handle, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        break;
    default:
        LEMON_UNREACHABLE();
    }

    mode = inMode;
}

void
MouseListener::processButtonEvent(MouseButton btn, KeyEvent evt, KeyMod mods)
{
    using namespace magic_enum;

    logger::trace("mouse event: button=", enum_name(btn), " event=", enum_name(evt),
                  " mods=", flags::enum_name(mods));
}

void
MouseListener::mouseButtonCallback(WindowHandle handle, int iBtn, int iEvt, int iMods)
{
    auto btn = static_cast<MouseButton>(iBtn);
    auto evt = static_cast<KeyEvent>(iEvt);
    auto mods = static_cast<KeyMod>(iMods);

    get()->processButtonEvent(btn, evt, mods);
}
