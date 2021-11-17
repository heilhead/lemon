#include "mesh.h"
#include "common/DemoModelActor.h"
#include <lemon/game.h>
#include <lemon/render.h>

#include "render/passes/MainRenderPass.h"
#include "render/passes/DebugUIRenderPass.h"
#include "render/passes/PostProcessRenderPass.h"

using namespace lemon;
using namespace lemon::game;
using namespace lemon::device;
using namespace lemon::utils;
using namespace lemon::res;
using namespace lemon::scheduler;
using namespace lemon::render;
using namespace demo;

class DemoGameState : public GameState {
public:
    void
    onStart() override
    {
        LEMON_TRACE_FN();
    }

    void
    onStop() override
    {
        LEMON_TRACE_FN();
    }

    void
    onPause() override
    {
        LEMON_TRACE_FN();
    }

    void
    onResume() override
    {
        LEMON_TRACE_FN();
    }

    Transition
    onInput(const InputEvent& evt) override
    {
        if (evt.isKeyPress(KeyCode::Enter)) {
            logger::trace("GAME STATE ENTER!");
        }

        if (evt.isKeyPress(KeyCode::Escape)) {
            return Transition::pop();
        }

        return std::nullopt;
    }

    Transition
    onUI() override
    {
        Transition trans;

        ImGui::Begin("DemoGameState");

        ImGui::Text("Game state!");

        if (ImGui::Button("<- DemoMenuState")) {
            trans = Transition::pop();
        }

        ImGui::End();

        return trans;
    }
};

class DemoMenuState : public GameState {
public:
    void
    onStart() override
    {
        LEMON_TRACE_FN();
    }

    void
    onStop() override
    {
        LEMON_TRACE_FN();
    }

    void
    onPause() override
    {
        LEMON_TRACE_FN();
    }

    void
    onResume() override
    {
        LEMON_TRACE_FN();
    }

    Transition
    onInput(const InputEvent& evt) override
    {
        if (evt.isKeyPress(KeyCode::Enter)) {
            return Transition::push<DemoGameState>();
        }

        if (evt.isKeyPress(KeyCode::Escape)) {
            return Transition::pop();
        }

        return std::nullopt;
    }

    Transition
    onUI() override
    {
        Transition trans;

        ImGui::Begin("DemoMenuState");

        ImGui::Text("Menu state!");

        if (ImGui::Button("-> DemoGameState")) {
            trans = Transition::push<DemoGameState>();
        }

        if (ImGui::Button("<- DemoRootState")) {
            trans = Transition::pop();
        }

        ImGui::End();

        return trans;
    }
};

class DemoRootState : public GameState {
    bool bShowDemoWindow = true;

public:
    void
    onStart() override
    {
        LEMON_TRACE_FN();
    }

    void
    onStop() override
    {
        LEMON_TRACE_FN();
    }

    void
    onPause() override
    {
        LEMON_TRACE_FN();
    }

    void
    onResume() override
    {
        LEMON_TRACE_FN();
    }

    Transition
    onInput(const InputEvent& evt) override
    {
        if (evt.isKeyPress(KeyCode::Enter)) {
            return Transition::push<DemoMenuState>();
        }

        if (evt.isKeyPress(KeyCode::Escape)) {
            return Transition::shutdown();
        }

        return std::nullopt;
    }

    void
    onShadowUpdate(float dt) override
    {
    }

    Transition
    onUI() override
    {
        Transition trans;

        ImGui::Begin("DemoRootState");

        ImGui::Text("Root state!");

        if (ImGui::Button("-> DemoMenuState")) {
            trans = Transition::push<DemoMenuState>();
        }

        if (ImGui::Button("<- Quit")) {
            trans = Transition::shutdown();
        }

        ImGui::End();

        return trans;
    }

    void
    onShadowUI() override
    {
        // if (bShowDemoWindow)
        //     ImGui::ShowDemoWindow(&bShowDemoWindow);
    }
};

glm::f32vec3
hsv2rgb(glm::f32vec3 hsv)
{
    double h = hsv.x, s = hsv.y, v = hsv.z, p, q, t, fract;

    if (h == 360.f) {
        h = 0.f;
    } else {
        h /= 60.f;
    };

    fract = h - std::floor(h);

    p = v * (1. - s);
    q = v * (1. - s * fract);
    t = v * (1. - s * (1. - fract));

    if (0.f <= h && h < 1.f)
        return {v, t, p};
    else if (1.f <= h && h < 2.f)
        return {q, v, p};
    else if (2.f <= h && h < 3.f)
        return {p, v, t};
    else if (3.f <= h && h < 4.f)
        return {p, q, v};
    else if (4.f <= h && h < 5.f)
        return {t, p, v};
    else if (5.f <= h && h < 6.f)
        return {v, p, q};
    else
        return {0.f, 0.f, 0.f};
}

namespace minirender {
    const ModelResource::Model*
    loadModel(const std::string& path)
    {
        ResourceLocation location(path);

        auto result = Scheduler::get()->block(
            ResourceManager::get()->loadResource<ModelResource>(location, ResourceLifetime::Static));

        LEMON_ASSERT(result.has_value());

        return (*result)->getObject<ModelResource::Model>(location.object);
    }

    const MaterialResource*
    loadMaterial(const std::string& path)
    {
        ResourceLocation location(path);

        auto result = Scheduler::get()->block(
            ResourceManager::get()->loadResource<MaterialResource>(location, ResourceLifetime::Static));

        LEMON_ASSERT(result.has_value());

        return *result;
    }
} // namespace minirender

class MiniRender {
public:
    MiniRender() {}

private:
    MaterialUniformData* pSharedData;

    std::chrono::time_point<std::chrono::steady_clock> timeStart = std::chrono::steady_clock::now();

    std::unique_ptr<GameWorld> pWorld;
    GameObjectHandle<FlyingCameraActor> hCameraActor;

    std::vector<GameObjectHandle<DemoModelActor>> demoActorHandles;

    std::unique_ptr<GameStateManager> pGameStateMan;

public:
    void
    init(Window* window)
    {
        using namespace minirender;

        auto model = loadModel("ozz-sample\\MannequinSkeleton.lem:SK_Mannequin");
        auto& meshData = model->getMeshes()[0];
        auto* pMaterial = loadMaterial("misc\\M_Mannequin2");
        auto material =
            MaterialManager::get()->getSurfaceMaterialInstance(*pMaterial, meshData.pMesh->vertexFormat);

        pSharedData = &PipelineManager::get()->getSurfaceUniformData();
        pWorld = std::make_unique<GameWorld>();

        hCameraActor = pWorld->createActor<FlyingCameraActor>(kVectorZAxis * -1500.f);

        auto* pCameraActor = hCameraActor.get();
        pCameraActor->activateCamera();
        pCameraActor->setSensitivity(3.f);

        static constexpr float spreadFactor = 100.f;

        int maxRings = 10;
        for (int i = 1; i < maxRings; i++) {
            int maxActors = i * 4;
            for (int j = 0; j < maxActors; j++) {
                float angleDeg = 360.f / maxActors * j;
                float angleRad = glm::radians(angleDeg);

                glm::f32vec3 pos{std::sin(angleRad), 0.f, std::cos(angleRad)};

                auto hsv = glm::f32vec3(angleDeg, static_cast<float>(i) / static_cast<float>(maxRings), 1.f);
                auto rgb = hsv2rgb(hsv);

                auto hDemoActor = pWorld->createActor<demo::DemoModelActor>(
                    pos * i * spreadFactor, glm::quatLookAt(-pos, kVectorUp) * kQuatUEOrientation);

                auto* pDemoActor = hDemoActor.get();
                pDemoActor->setColor(rgb);
                pDemoActor->setWave(i);

                demoActorHandles.emplace_back(hDemoActor);
            }
        }
    }

    void
    update(float dt)
    {
        constexpr auto cameraParam = lemon::sid("sceneParams.camera");
        constexpr auto timeParam = lemon::sid("sceneParams.time");

        std::chrono::duration<double> dur = std::chrono::steady_clock::now() - timeStart;
        double dTime = dur.count();                                  // time in seconds
        float fTime = static_cast<float>(dTime);                     // time in seconds, float
        float fTimeFrac = static_cast<float>(std::fmod(dTime, 1.f)); // fractional part

        pWorld->updateInternal(dTime);

        pSharedData->setData(cameraParam, pWorld->getCamera().getUniformData());
        pSharedData->setData(timeParam, glm::f32vec2(fTime, fTimeFrac));
    }

    void
    render()
    {
        Scheduler::get()->block(CPUTask(RenderManager::get()->render()));
    }
};

void
testMeshRendering()
{
    using namespace std::chrono_literals;

    lemon::Engine engine;

    engine.init(R"(C:\git\lemon\resources)");

    auto* pPostProcessMaterial = minirender::loadMaterial("internal\\materials\\M_PostProcess");
    auto postProcessMaterial = MaterialManager::get()->getPostProcessMaterialInstance(*pPostProcessMaterial);

    auto* pRenderMan = RenderManager::get();
    pRenderMan->addRenderPass(std::make_unique<MainRenderPass>());
    pRenderMan->addRenderPass(std::make_unique<PostProcessRenderPass>(std::move(postProcessMaterial)));
    pRenderMan->addRenderPass(std::make_unique<DebugUIRenderPass>());

    auto& ui = pRenderMan->getDebugUI();

    {
        MiniRender render;
        render.init(Device::get()->getWindow());

        auto pGameStateMan = std::make_unique<GameStateManager>();
        pGameStateMan->init(std::make_unique<DemoRootState>());

        engine.loop([&](float dt) {
            if (LoopControl::Abort == pGameStateMan->onInput()) {
                return LoopControl::Abort;
            }

            pGameStateMan->onPreUpdate(dt);

            render.update(dt);

            if (LoopControl::Abort == pGameStateMan->onPostUpdate(dt)) {
                return LoopControl::Abort;
            }

            if (ui.isEnabled()) {
                ui.update();

                if (LoopControl::Abort == pGameStateMan->onUI()) {
                    return LoopControl::Abort;
                }
            }

            render.render();

            return LoopControl::Continue;
        });
    }

    engine.shutdown();
}
