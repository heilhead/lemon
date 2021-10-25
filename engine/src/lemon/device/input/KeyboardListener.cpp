#include <lemon/device/input/KeyboardListener.h>

using namespace lemon;
using namespace lemon::device;

KeyboardListener::KeyboardListener(WindowHandle inHandle)
{
    handle = inHandle;
    glfwSetKeyCallback(handle, keyPressCallback);
}

void
KeyboardListener::processKeyEvent(KeyCode key, KeyEvent evt, KeyMod mods)
{
    getDelegate(key).invoke(evt, mods);
}

void
KeyboardListener::keyPressCallback(WindowHandle handle, int iKey, int iScanCode, int iEvt, int iMods)
{
    auto key = static_cast<KeyCode>(iKey);
    auto evt = static_cast<KeyEvent>(iEvt);
    auto mods = static_cast<KeyMod>(iMods);

    get()->processKeyEvent(key, evt, mods);
}

MulticastDelegate<KeyEvent, KeyMod>&
KeyboardListener::getDelegate(KeyCode key)
{
    auto search = keyListeners.find(key);
    if (search != std::end(keyListeners)) {
        return search->second;
    }

    auto [iter, bInserted] = keyListeners.emplace(key, MulticastDelegate<KeyEvent, KeyMod>());
    return iter->second;
}

KeyEvent
KeyboardListener::getKeyState(KeyCode key) const
{
    return static_cast<KeyEvent>(glfwGetKey(handle, static_cast<int>(key)));
}

bool
KeyboardListener::isKeyPressed(KeyCode key) const
{
    const auto state = getKeyState(key);
    return state == KeyEvent::Press || state == KeyEvent::Repeat;
}
