#pragma once

#include <lemon/game/state/InputEvent.h>

namespace lemon::game {
    class GameState;

    template<class T>
    concept GameStateBase = Base<T, GameState>;

    class GameState : NonCopyable {
    public:
        enum class TransitionType { None, Push, Pop, Swap, Clear, Shutdown };

        class Transition {
            friend class GameStateManager;

            std::unique_ptr<GameState> newState;
            TransitionType type;

        public:
            constexpr Transition() : type{TransitionType::None}, newState{nullptr} {}
            constexpr Transition(std::nullopt_t) : type{TransitionType::None}, newState{nullptr} {}
            constexpr Transition(TransitionType type) : type{type}, newState{nullptr} {}

            Transition(TransitionType type, std::unique_ptr<GameState> newState)
                : type{type}, newState{std::move(newState)}
            {
            }

            static Transition
            none()
            {
                return Transition{TransitionType::None};
            }

            static Transition
            push(std::unique_ptr<GameState> newState)
            {
                return Transition{TransitionType::Push, std::move(newState)};
            }

            template<GameStateBase TGameState, typename... TArgs>
            static Transition
            push(TArgs&&... args)
            {
                return Transition{TransitionType::Push,
                                  std::make_unique<TGameState>(std::forward<TArgs>(args)...)};
            }

            static Transition
            pop()
            {
                return Transition{TransitionType::Pop};
            }

            static Transition
            swap(std::unique_ptr<GameState> newState)
            {
                return Transition{TransitionType::Swap, std::move(newState)};
            }

            template<GameStateBase TGameState, typename... TArgs>
            static Transition
            swap(TArgs&&... args)
            {
                return Transition{TransitionType::Swap,
                                  std::make_unique<TGameState>(std::forward<TArgs>(args)...)};
            }

            static Transition
            clear(std::unique_ptr<GameState> newState)
            {
                return Transition{TransitionType::Clear, std::move(newState)};
            }

            template<GameStateBase TGameState, typename... TArgs>
            static Transition
            clear(TArgs&&... args)
            {
                return Transition{TransitionType::Clear,
                                  std::make_unique<TGameState>(std::forward<TArgs>(args)...)};
            }

            static Transition
            shutdown()
            {
                return Transition{TransitionType::Shutdown};
            }
        };

        virtual void
        onStart()
        {
        }

        virtual void
        onStop()
        {
        }

        virtual void
        onPause()
        {
        }

        virtual void
        onResume()
        {
        }

        virtual void
        onPreUpdate(float dt)
        {
        }

        virtual Transition
        onPostUpdate(float dt)
        {
            return Transition::none();
        }

        virtual void
        onShadowUpdate(float dt)
        {
        }

        virtual Transition
        onInput(const InputEvent& evt)
        {
            return Transition::none();
        }

        virtual void
        onShadowInput(const InputEvent& evt)
        {
        }

        virtual Transition
        onUI()
        {
            return Transition::none();
        }

        virtual void
        onShadowUI()
        {
        }
    };
} // namespace lemon::game
