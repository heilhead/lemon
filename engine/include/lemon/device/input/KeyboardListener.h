#pragma once

#include <GLFW/glfw3.h>
#include <lemon/device/common.h>
#include <lemon/device/input/common.h>
#include <lemon/misc/Delegate.h>

namespace lemon::device {
    class KeyboardListener : public UnsafeSingleton<KeyboardListener> {
        std::unordered_map<KeyCode, MulticastDelegate<KeyEvent, KeyMod>> keyListeners;
        WindowHandle handle;

    public:
        KeyboardListener(WindowHandle handle);

        MulticastDelegate<KeyEvent, KeyMod>&
        getDelegate(KeyCode key);

        KeyEvent
        getKeyState(KeyCode key) const;

        bool
        isKeyPressed(KeyCode key) const;

    private:
        void
        processKeyEvent(KeyCode key, KeyEvent evt, KeyMod mods);

        static void
        keyPressCallback(WindowHandle handle, int key, int scanCode, int evt, int mods);
    };
} // namespace lemon::device
