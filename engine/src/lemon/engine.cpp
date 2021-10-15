#include <lemon/engine.h>
#include <lemon/device/Window.h>
#include <lemon/shared/utils.h>

using namespace lemon;
using namespace lemon::device;

void
printGLFWError(int code, const char* message)
{
    logger::err("GLFW error: ", code, " - ", message);
}

void
Engine::init(std::string& assetPath)
{
    glfwSetErrorCallback(printGLFWError);

    if (!glfwInit()) {
        utils::halt("GLFW init failed");
    }
    
    schedMan = std::make_unique<scheduler::Scheduler>();
    resMan = std::make_unique<res::ResourceManager>(assetPath);
    device = std::make_unique<device::Device>();

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
