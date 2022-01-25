#include <lemon/engine.h>
#include <lemon/device/Window.h>

using namespace lemon;
using namespace lemon::device;
using namespace lemon::render;
using namespace lemon::res;
using namespace lemon::game;

void
printGLFWError(int code, gsl::czstring<> message)
{
    logger::err("GLFW error: ", code, " - ", message);
}

void
Engine::init(const std::string& assetPath, std::unique_ptr<game::RootGameState> rootState)
{
    glfwSetErrorCallback(printGLFWError);

    if (!glfwInit()) {
        utils::halt("GLFW init failed");
    }

    schedMan = std::make_unique<Scheduler>();
    resMan = std::make_unique<ResourceManager>(assetPath);
    device = std::make_unique<Device>();
    gameStateMan = std::make_unique<GameStateManager>();
    gameWorld = std::make_unique<GameWorld>();

    RenderManager::get()->getDebugUI().enable();
    gameStateMan->init(std::move(rootState));

    logger::log("initialization complete!");
}

void
Engine::loop()
{
    logger::log("entering event loop");

    auto* pScheduler = Scheduler::get();
    auto* pRenderMan = RenderManager::get();
    auto* pDebugUI = &pRenderMan->getDebugUI();
    auto* pGameStateMan = gameStateMan.get();
    auto* pGameWorld = gameWorld.get();

    std::chrono::time_point<std::chrono::steady_clock> timeStart = std::chrono::steady_clock::now();

    device->getWindow()->loop([=](auto) {
        OPTICK_FRAME("GameThread");

        std::chrono::duration<double> dur = std::chrono::steady_clock::now() - timeStart;

        // Delta time in seconds.
        double dt = dur.count();

        device->getInput()->update();

        if (LoopControl::Abort == pGameStateMan->onInput()) {
            return LoopControl::Abort;
        }

        pGameStateMan->onPreUpdate(dt);

        if (LoopControl::Abort == pGameStateMan->onPostUpdate(dt)) {
            return LoopControl::Abort;
        }

        pGameWorld->updateInternal(dt);

        // Render the debug UI.
        // TODO: This is temporary.
        if (pDebugUI->isEnabled()) {
            pDebugUI->update();

            if (LoopControl::Abort == pGameStateMan->onUI()) {
                return LoopControl::Abort;
            }
        }

        {
            RenderManager::FrameWorldContext ctx;
            ctx.camera = pGameWorld->getCamera().getUniformData();
            ctx.dt = dt;

            // Dispatch frame render.
            pRenderMan->beginFrame(ctx);
        }

        // Process game thread tasks while the render thread is busy rendering.
        pScheduler->processGameThreadTasks();

        // Synchronize with the render thread and the end of the frame.
        pRenderMan->endFrame();

        return LoopControl::Continue;
    });

    logger::log("exiting event loop");
}

void
Engine::shutdown()
{
    // Let the resources release any keep-alive handles before the stores are destroyed.
    resMan->getStore().clear();

    RenderManager::get()->releaseResources();

    glfwTerminate();
}
