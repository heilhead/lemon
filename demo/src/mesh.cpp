#include "mesh.h"
#include "common/DemoModelActor.h"
#include <lemon/game.h>

using namespace lemon;
using namespace lemon::game;
using namespace lemon::device;
using namespace lemon::utils;
using namespace lemon::res;
using namespace lemon::scheduler;
using namespace lemon::render;
using namespace demo;

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
    loadModel()
    {
        ResourceLocation location(R"(ozz-sample\MannequinSkeleton.lem:SK_Mannequin)");

        auto result = Scheduler::get()->block(
            ResourceManager::get()->loadResource<ModelResource>(location, ResourceLifetime::Static));

        LEMON_ASSERT(result.has_value());

        return (*result)->getObject<ModelResource::Model>(location.object);
    }

    const MaterialResource*
    loadMaterial()
    {
        ResourceLocation location(R"(misc\M_Mannequin2)");

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
    std::unique_ptr<lemon::device::Platform> platform;

    wgpu::Device device;
    wgpu::Buffer indexBuffer;
    wgpu::Buffer vertexBuffer;
    wgpu::Queue queue;
    wgpu::SwapChain swapChain;
    wgpu::TextureView depthStencilView;

    MaterialUniformData* pSharedData;

    std::chrono::time_point<std::chrono::steady_clock> timeStart = std::chrono::steady_clock::now();

    std::unique_ptr<GameWorld> pWorld;
    std::unique_ptr<RenderQueue> pRenderQueue;
    GameObjectHandle<FlyingCameraActor> hCameraActor;

    std::vector<GameObjectHandle<DemoModelActor>> demoActorHandles;

public:
    void
    init(Window* window)
    {
        using namespace minirender;

        auto* gpu = Device::get()->getGPU();

        device = gpu->getDevice();
        swapChain = gpu->getSwapChain();
        queue = gpu->getQueue();

        const auto& swapChainImpl = gpu->getSwapChainImpl();
        auto [wndWidth, wndHeight] = window->getSize();

        depthStencilView = createDefaultDepthStencilView(device, wndWidth, wndHeight);

        auto model = loadModel();
        auto& meshData = model->getMeshes()[0];
        auto* pMaterial = loadMaterial();
        auto material = MaterialManager::get()->getMaterialInstance(*pMaterial, meshData.pMesh->vertexFormat);

        pSharedData = &PipelineManager::get()->getSharedUniformData();
        pWorld = std::make_unique<GameWorld>();
        pRenderQueue = std::make_unique<RenderQueue>();

        hCameraActor = pWorld->createActor<FlyingCameraActor>(kVectorZAxis * -1500.f);
        hCameraActor.get()->activateCamera();

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
        auto* pPipelineMan = PipelineManager::get();
        auto* pRenderMan = RenderManager::get();
        auto& renderQueue = GameWorld::get()->getRenderQueue();

        auto& cbuffer = pRenderMan->getConstantBuffer();
        cbuffer.reset();

        pSharedData->merge(cbuffer);

        wgpu::TextureView backbufferView = swapChain.GetCurrentTextureView();
        ComboRenderPassDescriptor renderPass({backbufferView}, depthStencilView);

        wgpu::CommandEncoder encoder = device.CreateCommandEncoder();

        {
            wgpu::RenderPassEncoder pass = encoder.BeginRenderPass(&renderPass);

            constexpr auto matModel = lemon::sid("packetParams.matModel");

            for (auto& renderProxy : renderQueue.getMeshes()) {
                renderProxy.pOwner->updateRenderProxy(renderProxy);

                auto& mat = renderProxy.material;
                auto& matData = mat.getUniformData();
                matData.setData(matModel, renderProxy.matrix);
                matData.merge(cbuffer);

                pass.SetPipeline(mat.getRenderPipeline().getColorPipeline());

                pass.SetBindGroup(kSharedBindGroupIndex, pPipelineMan->getSharedBindGroup(),
                                  pSharedData->getOffsetCount(), pSharedData->getOffsets());

                pass.SetBindGroup(kMaterialBindGroupIndex, mat.getBindGroup(), matData.getOffsetCount(),
                                  matData.getOffsets());

                pass.SetVertexBuffer(0, renderProxy.vertexBuffer);
                pass.SetIndexBuffer(renderProxy.indexBuffer, renderProxy.indexFormat);
                pass.DrawIndexed(renderProxy.indexCount);
            }

            pass.EndPass();
        }

        cbuffer.upload(pRenderMan->getDevice());

        wgpu::CommandBuffer commands = encoder.Finish();
        queue.Submit(1, &commands);
        swapChain.Present();
    }
};

void
testMeshRendering()
{
    lemon::Engine engine;

    engine.init(R"(C:\git\lemon\resources)");

    {
        MiniRender render;
        render.init(Device::get()->getWindow());
        engine.loop([&](float dt) {
            render.update(dt);
            render.render();

            if (KeyboardListener::get()->isKeyPressed(KeyCode::Escape)) {
                return LoopControl::Abort;
            }

            return LoopControl::Continue;
        });
    }

    engine.shutdown();
}
