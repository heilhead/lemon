#pragma once

#include <lemon/game/state/GameState.h>
#include <lemon/game/state/InputEvent.h>
#include <lemon/device/common.h>
#include <lemon/misc/Delegate.h>

namespace lemon::game {
    class GameStateManager : public UnsafeSingleton<GameStateManager> {
        using Control = device::LoopControl;

        static constexpr size_t kMaxInlineStates = 8;
        static constexpr size_t kMaxInlineInputEvents = 16;

        folly::small_vector<std::unique_ptr<GameState>, kMaxInlineStates> stateStack;
        folly::small_vector<InputEvent, kMaxInlineInputEvents> inputQueue;

        DelegateHandle hKeyboardListener;
        DelegateHandle hMouseListener;

    public:
        GameStateManager();

        ~GameStateManager();

        void
        init(std::unique_ptr<GameState> rootState);

        void
        onPreUpdate(float dt);

        Control
        onPostUpdate(float dt);

        Control
        onUI();

        Control
        onInput();

    private:
        Control
        processTansition(GameState::Transition trans);

        void
        push(std::unique_ptr<GameState> newState);

        void
        pop();

        void
        swap(std::unique_ptr<GameState> newState);

        void
        clear(std::unique_ptr<GameState> newState);

        void
        clearInternal();

        void
        handleKeyboardEvent(device::KeyCode keyCode, device::KeyEvent evt, device::KeyMod mods);

        void
        handleMouseEvent(device::MouseButton btn, device::KeyEvent evt, device::KeyMod mods);
    };
} // namespace lemon::game
