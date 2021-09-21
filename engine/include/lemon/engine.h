#pragma once

#include <memory>
#include <string>
#include <functional>

#include <lemon/resource/ResourceManager.h>
#include <lemon/scheduler.h>
#include <lemon/device/Window.h>

namespace lemon {
    class Engine final {
    public:
        Engine();

    private:
        std::unique_ptr<lemon::scheduler::Scheduler> schedMan;
        std::unique_ptr<lemon::res::ResourceManager> resMan;
        std::unique_ptr<lemon::Window> wnd;

    public:
        void
        init(std::string& assetPath);

        void
        loop(const std::function<LoopControl(float)>& callback);

        void
        shutdown();

        inline lemon::Window*
        getWindow() {
            return wnd.get();
        }

        static Engine*
        get();
    };
} // namespace lemon
