#pragma once

#include <lemon/resource/ResourceManager.h>
#include <lemon/scheduler.h>
#include <lemon/device/Device.h>
#include <lemon/game/state/GameStateManager.h>
#include <lemon/game/actor/GameWorld.h>

namespace lemon {
    class Engine final : public UnsafeSingleton<Engine> {
        std::unique_ptr<scheduler::Scheduler> schedMan;
        std::unique_ptr<res::ResourceManager> resMan;
        std::unique_ptr<device::Device> device;
        std::unique_ptr<game::GameStateManager> gameStateMan;
        std::unique_ptr<game::GameWorld> gameWorld;

    public:
        void
        init(const std::string& assetPath, std::unique_ptr<game::RootGameState> rootState);

        void
        loop();

        void
        shutdown();
    };
} // namespace lemon
