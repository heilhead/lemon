#include <lemon/game/state/GameStateManager.h>
#include <lemon/device/input/KeyboardListener.h>
#include <lemon/device/input/MouseListener.h>

using namespace lemon;
using namespace lemon::game;
using namespace lemon::device;

GameStateManager::GameStateManager() : stateStack{}
{
    hMouseListener = MouseListener::get()->getGlobalDelegate().add(&GameStateManager::handleMouseEvent, this);
    hKeyboardListener =
        KeyboardListener::get()->getGlobalDelegate().add(&GameStateManager::handleKeyboardEvent, this);
}

GameStateManager::~GameStateManager()
{
    // Required to call lifecylce events in the correct order.
    clearInternal();

    MouseListener::get()->getGlobalDelegate().remove(hMouseListener);
    KeyboardListener::get()->getGlobalDelegate().remove(hKeyboardListener);
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

    for (auto& state : stateStack) {
        state->onShadowUpdate(dt);
    }

    return processTansition(stateStack.back()->onPostUpdate(dt));
}

GameStateManager::Control
GameStateManager::onUI()
{
    if (stateStack.size()) {
        for (auto& state : stateStack) {
            state->onShadowUI();
        }

        return processTansition(stateStack.back()->onUI());
    }

    return Control::Continue;
}

GameStateManager::Control
GameStateManager::onInput()
{
    for (auto& evt : inputQueue) {
        if (stateStack.size()) {
            for (auto& state : stateStack) {
                state->onShadowInput(evt);
            }

            if (processTansition(stateStack.back()->onInput(evt)) == Control::Abort) {
                return Control::Abort;
            }
        }
    }

    inputQueue.clear();

    return Control::Continue;
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

void
GameStateManager::handleKeyboardEvent(device::KeyCode keyCode, device::KeyEvent evt, device::KeyMod mods)
{
    inputQueue.emplace_back(keyCode, evt, mods);
}

void
GameStateManager::handleMouseEvent(device::MouseButton btn, device::KeyEvent evt, device::KeyMod mods)
{
    inputQueue.emplace_back(btn, evt, mods);
}
