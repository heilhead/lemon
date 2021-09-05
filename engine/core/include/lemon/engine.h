#pragma once

namespace lemon {
    enum class EngineState {
        None,
        Idle,
        Running,
    };

    class Engine final {
    public:
        Engine();

        void init();
        void shutdown();

        static Engine* get();

    private:
        EngineState state = EngineState::None;
    };
}

