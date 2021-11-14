#include <lemon/device/input/MouseListener.h>

using namespace lemon;
using namespace lemon::device;

inline void
setRawInputMode(WindowHandle handle, bool bEnabled)
{
    if (glfwRawMouseMotionSupported()) {
        if (bEnabled) {
            glfwSetInputMode(handle, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
        } else {
            glfwSetInputMode(handle, GLFW_RAW_MOUSE_MOTION, GLFW_FALSE);
        }
    }
}

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
        setRawInputMode(handle, false);
        glfwSetInputMode(handle, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        break;
    case lemon::device::CursorMode::Disabled:
        glfwSetInputMode(handle, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        setRawInputMode(handle, false);
        break;
    case lemon::device::CursorMode::Raw:
        glfwSetInputMode(handle, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        setRawInputMode(handle, true);
        break;
    default:
        LEMON_UNREACHABLE();
    }

    mode = inMode;
}

void
MouseListener::processButtonEvent(MouseButton btn, KeyEvent evt, KeyMod mods)
{
    getGlobalDelegate().invoke(btn, evt, mods);
    getDelegate(btn).invoke(evt, mods);
}

void
MouseListener::mouseButtonCallback(WindowHandle handle, int iBtn, int iEvt, int iMods)
{
    auto btn = static_cast<MouseButton>(iBtn);
    auto evt = static_cast<KeyEvent>(iEvt);
    auto mods = static_cast<KeyMod>(iMods);
    get()->processButtonEvent(btn, evt, mods);
}

MulticastDelegate<KeyEvent, KeyMod>&
MouseListener::getDelegate(MouseButton btn)
{
    auto search = buttonListeners.find(btn);
    if (search != std::end(buttonListeners)) {
        return search->second;
    }

    auto [iter, bInserted] = buttonListeners.emplace(btn, MulticastDelegate<KeyEvent, KeyMod>());
    return iter->second;
}

MulticastDelegate<MouseButton, KeyEvent, KeyMod>&
MouseListener::getGlobalDelegate()
{
    return globalListeners;
}
