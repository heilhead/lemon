#include "mesh.h"
#include "game/Transform.h"
#include "game/Camera.h"
#include "game/print.h"

using namespace lemon;
using namespace lemon::device;
using namespace lemon::utils;
using namespace lemon::res;
using namespace lemon::scheduler;
using namespace lemon::render;
using namespace game;

namespace minirender {
    using MeshHandle = const ModelResource::Mesh*;

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
        ResourceLocation location(R"(misc\\M_Mannequin2)");

        auto result = Scheduler::get()->block(
            ResourceManager::get()->loadResource<MaterialResource>(location, ResourceLifetime::Static));

        LEMON_ASSERT(result.has_value());

        return *result;
    }

    struct DrawCall {
        MeshHandle mesh;
        MaterialInstance material;
        wgpu::Buffer vbuf;
        wgpu::Buffer ibuf;
        Transform transform;
    };
} // namespace minirender

class MiniRender {
public:
    MiniRender() {}

private:
    std::unique_ptr<lemon::device::Platform> platform;

    wgpu::Device device;
    wgpu::Buffer indexBuffer;
    wgpu::Buffer vertexBuffer;
    wgpu::Texture texture;
    wgpu::Sampler sampler;
    wgpu::Queue queue;
    wgpu::SwapChain swapChain;
    wgpu::TextureView depthStencilView;
    wgpu::RenderPipeline pipeline;
    wgpu::BindGroup bindGroup;
    wgpu::BindGroup bindGroupShared;

    MaterialUniformData* pSharedData;

    minirender::MeshHandle mesh;

    std::array<minirender::DrawCall, 4> drawCalls;
    Camera camera;
    std::chrono::time_point<std::chrono::steady_clock> timeStart = std::chrono::steady_clock::now();

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
        mesh = model->getMeshes()[0];

        auto* pMaterial = loadMaterial();

        vertexBuffer = createBufferFromData(device, mesh->vertexData, mesh->vertexData.size(),
                                            wgpu::BufferUsage::Vertex);
        indexBuffer =
            createBufferFromData(device, mesh->indexData, mesh->indexData.size(), wgpu::BufferUsage::Index);

        auto material = MaterialManager::get()->getMaterialInstance(*pMaterial, mesh->vertexFormat);

        // auto camData = camera.getUniformData();
        // camData.matView = glm::lookAt(glm::f32vec3(200.f), glm::f32vec3(0.f), Transform::up);

        pSharedData = &PipelineManager::get()->getSharedUniformData();

        {
            constexpr auto tint = lemon::sid("materialParams.tint");
            constexpr auto colWhite = glm::f32vec4(1.f, 1.f, 1.f, 1.f);
            constexpr auto colRed = glm::f32vec4(1.f, 0.f, 0.f, 1.f);
            constexpr auto colGreen = glm::f32vec4(0.f, 1.f, 0.f, 1.f);
            constexpr auto colBlue = glm::f32vec4(0.f, 0.f, 1.f, 1.f);

            auto idx = 0;
            {
                DrawCall dc{.mesh = mesh,
                            .material = material,
                            .vbuf = vertexBuffer,
                            .ibuf = indexBuffer,
                            .transform = Transform{}};
                dc.material.setParameter(tint, colWhite);
                dc.transform.setPosition(0.f, 0.f, 0.f);
                dc.transform.setScale(0.5f, 0.5f, 0.5f);
                dc.transform.setRotation(math::toOrientationQuat(kVectorLeft));
                drawCalls[idx++] = std::move(dc);
            }

            {
                DrawCall dc{.mesh = mesh,
                            .material = material,
                            .vbuf = vertexBuffer,
                            .ibuf = indexBuffer,
                            .transform = Transform{}};
                dc.material.setParameter(tint, colRed);
                dc.transform.setPosition(kVectorForward * 500.f);
                // dc.transform.setPosition(kVectorXAxis * 500.f);
                dc.transform.setScale(0.5f, 0.5f, 0.5f);
                // dc.transform.lookAt(0.f, 200.f, 0.f);
                // dc.transform.rotate(90.f, 0.f, -90.f);
                drawCalls[idx++] = std::move(dc);
            }

            {
                DrawCall dc{.mesh = mesh,
                            .material = material,
                            .vbuf = vertexBuffer,
                            .ibuf = indexBuffer,
                            .transform = Transform{}};
                dc.material.setParameter(tint, colGreen);
                dc.transform.setPosition(kVectorLeft * 500.f);
                // dc.transform.setPosition(kVectorYAxis * 500.f);
                dc.transform.setScale(0.5f, 0.5f, 0.5f);
                // dc.transform.lookAt(0.f, 200.f, 0.f);
                // dc.transform.rotate(90.f, 0.f, -90.f);
                drawCalls[idx++] = std::move(dc);
            }

            {
                DrawCall dc{.mesh = mesh,
                            .material = material,
                            .vbuf = vertexBuffer,
                            .ibuf = indexBuffer,
                            .transform = Transform{}};
                dc.material.setParameter(tint, colBlue);
                dc.transform.setPosition(kVectorUp * 500.f);
                // dc.transform.setPosition(kVectorZAxis * 500.f);
                dc.transform.setScale(0.5f, 0.5f, 0.5f);
                // dc.transform.lookAt(0.f, 200.f, 0.f);
                // dc.transform.rotate(90.f, 0.f, -90.f);
                drawCalls[idx++] = std::move(dc);
            }
        }

        camera.setClipping(1.f, 100000.f);
        camera.setView(wndWidth, wndHeight);
    }

    void
    update(float dt)
    {
        static constexpr auto cameraParam = lemon::sid("sceneParams.camera");
        static constexpr auto timeParam = lemon::sid("sceneParams.time");

        std::chrono::duration<float> dur = std::chrono::steady_clock::now() - timeStart;
        auto fTime = dur.count(); // time in seconds

        auto& tCamera = camera.getTransform();
        // tCamera.setPosition(kVectorXAxis * 3000.f);
        // tCamera.setPosition(kVectorXAxis * (std::sin(fTime) * 1500.f + 1500.f));
        // tCamera.setRotation(0.f, 90.f, 0.f);
        // tCamera.setRotation(math::toOrientationQuat(kVectorForward));

        auto camPos = kVectorXAxis * (std::sin(fTime) * 750.f);

        auto& udCamera = camera.getUniformData();
        udCamera.matView = glm::lookAt(camPos, kVectorZero, kVectorZAxis);

        logger::trace("pos: ", camPos);

        pSharedData->setData(cameraParam, udCamera);
        pSharedData->setData(timeParam, fTime);
    }

    void
    render()
    {
        auto* pPipelineMan = PipelineManager::get();
        auto* pRenderMan = RenderManager::get();

        auto& cbuffer = pRenderMan->getConstantBuffer();
        cbuffer.reset();

        pSharedData->merge(cbuffer);

        wgpu::TextureView backbufferView = swapChain.GetCurrentTextureView();
        ComboRenderPassDescriptor renderPass({backbufferView}, depthStencilView);

        wgpu::CommandEncoder encoder = device.CreateCommandEncoder();

        {
            wgpu::RenderPassEncoder pass = encoder.BeginRenderPass(&renderPass);

            for (auto& dc : drawCalls) {
                constexpr auto matModel = lemon::sid("packetParams.matModel");

                auto& mat = dc.material;
                auto& matData = mat.getUniformData();
                matData.setData(matModel, dc.transform.getMatrix());
                matData.merge(cbuffer);

                pass.SetPipeline(mat.getRenderPipeline().getColorPipeline());

                pass.SetBindGroup(kSharedBindGroupIndex, pPipelineMan->getSharedBindGroup(),
                                  pSharedData->getOffsetCount(), pSharedData->getOffsets());

                pass.SetBindGroup(kMaterialBindGroupIndex, mat.getBindGroup(), matData.getOffsetCount(),
                                  matData.getOffsets());

                pass.SetVertexBuffer(0, dc.vbuf);
                pass.SetIndexBuffer(dc.ibuf, dc.mesh->indexFormat);
                pass.DrawIndexed(dc.mesh->indexCount);
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
            return LoopControl::Continue;
        });
    }

    engine.shutdown();
}
