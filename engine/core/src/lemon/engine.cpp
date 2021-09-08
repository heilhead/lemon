#include "lemon/engine.h"

//#include <vector>
//#include <cstdint>
//#include <memory>
#include <cassert>
#include "lemon/prelude.h"
#include "lemon/device/Window.h"
#include "lemon/Triangle.h"

using namespace lemon;

std::unique_ptr<Engine> gEngine = nullptr;

void printGLFWError(int code, const char* message) {
	utils::printErr("GLFW error: ", code, " - ", message);
}

Engine::Engine() {
    assert(gEngine == nullptr);
}

void Engine::init() {
//    assert(state != EngineState::Running);
//    state = EngineState::Idle;

	glfwSetErrorCallback(printGLFWError);

	if (!glfwInit()) {
		utils::halt("GLFW init failed");
	}

    {
        WindowDescriptor wndDesc;
        Window wnd(wndDesc);
        Triangle triangle(wnd);

        utils::print("initialization complete!");

        wnd.loop([&](auto) {
            triangle.render();
            return LoopControl::Continue;
        });
    }
}

void Engine::shutdown() {
//    assert(state == EngineState::Running);
//    state = EngineState::Idle;

    glfwTerminate();
}

Engine* Engine::get() {
    if (gEngine == nullptr) {
        gEngine = std::make_unique<Engine>();
    }

    return gEngine.get();
}
