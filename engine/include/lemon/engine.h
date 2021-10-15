#pragma once

#include <memory>
#include <string>
#include <functional>
#include <lemon/resource/ResourceManager.h>
#include <lemon/scheduler.h>
#include <lemon/device/Device.h>
#include <lemon/shared/UnsafeSingleton.h>

namespace lemon {
    class Engine final : public UnsafeSingleton<Engine> {
        std::unique_ptr<lemon::scheduler::Scheduler> schedMan;
        std::unique_ptr<lemon::res::ResourceManager> resMan;
        std::unique_ptr<lemon::device::Device> device;

    public:
        void
        init(std::string& assetPath);

        void
        loop(const std::function<lemon::device::LoopControl(float)>& callback);

        void
        shutdown();
    };
} // namespace lemon
