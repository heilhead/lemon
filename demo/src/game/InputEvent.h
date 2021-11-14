#pragma once

namespace lemon::game {
    struct InputEvent {
        enum class Type {
            KeyboardEvent,
            MouseEvent,
        };

        InputEvent(device::KeyCode keyCode, device::KeyEvent evt, device::KeyMod mods)
            : type{Type::KeyboardEvent}, code{static_cast<int>(keyCode)}, evt{evt}, mods{mods}
        {
        }

        InputEvent(device::MouseButton btn, device::KeyEvent evt, device::KeyMod mods)
            : type{Type::MouseEvent}, code{static_cast<int>(btn)}, evt{evt}, mods{mods}
        {
        }

        inline device::KeyEvent
        getKeyEvent() const
        {
            return evt;
        }

        inline device::KeyMod
        getKeyMods() const
        {
            return mods;
        }

        inline device::KeyCode
        getKeyCode() const
        {
            LEMON_ASSERT(isKeyboardEvent());
            return static_cast<device::KeyCode>(code);
        }

        inline device::MouseButton
        getMouseButton() const
        {
            LEMON_ASSERT(isMouseEvent());
            return static_cast<device::MouseButton>(code);
        }

        inline Type
        getType() const
        {
            return type;
        }

        inline bool
        isKeyboardEvent() const
        {
            return getType() == Type::KeyboardEvent;
        }

        inline bool
        isMouseEvent() const
        {
            return getType() == Type::MouseEvent;
        }

        inline bool
        isPress() const
        {
            return evt == device::KeyEvent::Press;
        }

        inline bool
        isRelease() const
        {
            return evt == device::KeyEvent::Release;
        }

        inline bool
        isRepeat() const
        {
            return evt == device::KeyEvent::Repeat;
        }

        inline bool
        isKeyCode(device::KeyCode keyCode) const
        {
            return isKeyboardEvent() && getKeyCode() == keyCode;
        }

        inline bool
        isMouseButton(device::MouseButton btn) const
        {
            return isMouseEvent() && getMouseButton() == btn;
        }

        inline bool
        isKeyPress(device::KeyCode keyCode) const
        {
            return isKeyCode(keyCode) && isPress();
        }

        inline bool
        isKeyRelease(device::KeyCode keyCode) const
        {
            return isKeyCode(keyCode) && isRelease();
        }

        inline bool
        isKeyRepeat(device::KeyCode keyCode) const
        {
            return isKeyCode(keyCode) && isRepeat();
        }

        inline bool
        isMouseButtonPress(device::MouseButton btn) const
        {
            return isMouseButton(btn) && isPress();
        }

        inline bool
        isMouseButtonRelease(device::MouseButton btn) const
        {
            return isMouseButton(btn) && isRelease();
        }

        inline bool
        isMouseButtonRepeat(device::MouseButton btn) const
        {
            return isMouseButton(btn) && isRepeat();
        }

    private:
        Type type;
        int code;
        device::KeyEvent evt;
        device::KeyMod mods;
    };
} // namespace lemon::game
