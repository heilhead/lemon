#include <lemon/engine.h>
#include <lemon/device/Window.h>
#include <lemon/shared/utils.h>

using namespace lemon;
using namespace lemon::device;

std::unique_ptr<Engine> gEngine = nullptr;

void
printGLFWError(int code, const char* message)
{
    logger::err("GLFW error: ", code, " - ", message);
}

Engine::Engine()
{
    assert(gEngine == nullptr);
}

void
Engine::init(std::string& assetPath)
{
    glfwSetErrorCallback(printGLFWError);

    if (!glfwInit()) {
        utils::halt("GLFW init failed");
    }

    device = std::make_unique<device::Device>();
    schedMan = std::make_unique<scheduler::Scheduler>();
    resMan = std::make_unique<res::ResourceManager>(assetPath);

    logger::log("initialization complete!");
}

void
Engine::loop(const std::function<LoopControl(float)>& callback)
{
    logger::log("entering event loop");
    device->getWindow()->loop([&](float dt) { return callback(dt); });
    logger::log("exiting event loop");
}

void
Engine::shutdown()
{
    // Let the resources release any keep-alive handles before the stores are destroyed.
    resMan->getStore().clear();

    glfwTerminate();
}

Engine*
Engine::get()
{
    if (gEngine == nullptr) {
        gEngine = std::make_unique<Engine>();
    }

    return gEngine.get();
}
