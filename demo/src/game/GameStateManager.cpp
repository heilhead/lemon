#include "GameStateManager.h"

using namespace lemon;
using namespace lemon::game;

GameStateManager::GameStateManager() : stateStack{} {}

GameStateManager::~GameStateManager()
{
    // Required to call lifecylce events in the correct order.
    clearInternal();
}

void
GameStateManager::init(std::unique_ptr<GameState> rootState)
{
    clear(std::move(rootState));
}

void
GameStateManager::onPreUpdate(float dt)
{
    if (stateStack.size()) {
        stateStack.back()->onPreUpdate(dt);
    }
}

GameStateManager::Control
GameStateManager::onPostUpdate(float dt)
{
    if (!stateStack.size()) {
        return Control::Continue;
    }

    auto ctrl = processTansition(stateStack.back()->onPostUpdate(dt));
    if (ctrl != Control::Abort) {
        for (auto& state : stateStack) {
            state->onShadowUpdate(dt);
        }
    }

    return ctrl;
}

void
GameStateManager::onDebugUI()
{
    if (stateStack.size()) {
        for (auto& state : stateStack) {
            state->onShadowDebugUI();
        }

        stateStack.back()->onDebugUI();
    }
}

GameStateManager::Control
GameStateManager::processTansition(GameState::Transition trans)
{
    switch (trans.type) {
    case GameState::TransitionType::Shutdown:
        return Control::Abort;
    case GameState::TransitionType::Push:
        push(std::move(trans.newState));
        break;
    case GameState::TransitionType::Pop:
        pop();
        break;
    case GameState::TransitionType::Swap:
        swap(std::move(trans.newState));
        break;
    case GameState::TransitionType::Clear:
        clear(std::move(trans.newState));
        break;
    }

    return Control::Continue;
}

void
GameStateManager::push(std::unique_ptr<GameState> newState)
{
    if (stateStack.size() > 0) {
        stateStack.back()->onPause();
    }

    stateStack.emplace_back(std::move(newState));
    stateStack.back()->onStart();
}

void
GameStateManager::pop()
{
    if (stateStack.size() > 0) {
        stateStack.back()->onStop();
        stateStack.pop_back();

        if (stateStack.size() > 0) {
            stateStack.back()->onResume();
        }
    }
}

void
GameStateManager::swap(std::unique_ptr<GameState> newState)
{
    if (stateStack.size() > 0) {
        stateStack.back()->onStop();
        stateStack.pop_back();
    }

    stateStack.emplace_back(std::move(newState));
    stateStack.back()->onStart();
}

void
GameStateManager::clear(std::unique_ptr<GameState> newState)
{
    clearInternal();
    push(std::move(newState));
}

void
GameStateManager::clearInternal()
{
    while (stateStack.size() > 0) {
        pop();
    }
}
