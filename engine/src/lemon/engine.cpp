#include <lemon/engine.h>

//#include <vector>
//#include <cstdint>
//#include <memory>
#include <cassert>
//#include <lemon/Triangle.h>
#include <lemon/device/Window.h>
#include <lemon/prelude.h>

using namespace lemon;

std::unique_ptr<Engine> gEngine = nullptr;

void
printGLFWError(int code, const char* message) {
    utils::logErr("GLFW error: ", code, " - ", message);
}

Engine::Engine() {
    assert(gEngine == nullptr);
}

void
Engine::init(std::string& assetPath) {
    glfwSetErrorCallback(printGLFWError);

    if (!glfwInit()) {
        utils::halt("GLFW init failed");
    }

    schedMan = std::make_unique<scheduler::Scheduler>();
    resMan = std::make_unique<res::ResourceManager>(assetPath);
    wnd = std::make_unique<Window>(WindowDescriptor{});

    utils::log("initialization complete!");
}

void
Engine::loop(const std::function<LoopControl(float)>& callback) {
    utils::log("entering event loop");

    wnd->loop([&](float dt) { return callback(dt); });

    utils::log("exiting event loop");
}

void
Engine::shutdown() {
    glfwTerminate();
}

Engine*
Engine::get() {
    if (gEngine == nullptr) {
        gEngine = std::make_unique<Engine>();
    }

    return gEngine.get();
}
