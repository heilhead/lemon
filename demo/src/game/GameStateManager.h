#pragma once

#include "GameState.h"

namespace lemon::game {
    class GameStateManager : public UnsafeSingleton<GameStateManager> {
        using Control = device::LoopControl;

        static constexpr size_t kMaxInlineStates = 8;

        folly::small_vector<std::unique_ptr<GameState>, kMaxInlineStates> stateStack;

    public:
        GameStateManager();

        ~GameStateManager();

        void
        init(std::unique_ptr<GameState> rootState);

        void
        onPreUpdate(float dt);

        Control
        onPostUpdate(float dt);

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
    };
} // namespace lemon::game
