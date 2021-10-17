#include "mesh.h"
#include "game/Transform.h"
#include "game/Camera.h"

using namespace lemon;
using namespace lemon::device;
using namespace lemon::utils;
using namespace lemon::res;
using namespace lemon::scheduler;
using namespace lemon::render;
using namespace game;

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
        ResourceLocation location(R"(misc\\M_Mannequin2)");

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
    wgpu::Texture texture;
    wgpu::Sampler sampler;
    wgpu::Queue queue;
    wgpu::SwapChain swapChain;
    wgpu::TextureView depthStencilView;
    wgpu::RenderPipeline pipeline;
    wgpu::BindGroup bindGroup;
    wgpu::BindGroup bindGroupShared;

    MaterialUniformData* pSharedData;
    MaterialUniformData* pMaterialData;
    MaterialInstance material;

    const model::ModelMesh* mesh;

public:
    void
    init(Window* window)
    {
        auto* gpu = Device::get()->getGPU();

        device = gpu->getDevice();
        swapChain = gpu->getSwapChain();
        queue = gpu->getQueue();

        const auto& swapChainImpl = gpu->getSwapChainImpl();
        auto [wndWidth, wndHeight] = window->getSize();

        Transform tModel;
        tModel.setPosition(0.f, 0.f, 0.f);
        tModel.setRotation(90.f, 225.0f, 0.f);
        tModel.setScale(1.f);

        Camera camera;
        camera.setClipping(1.f, 1000.f);
        camera.setView(wndWidth, wndHeight);

        auto& tCamera = camera.getTransform();
        tCamera.setPosition(0.f, -100.f, 100.f);
        tCamera.setRotation(0.f, 0.f, 0.f);

        depthStencilView = createDefaultDepthStencilView(device, wndWidth, wndHeight);

        auto model = minirender::loadModel();
        mesh = model->getMeshes()[0];

        auto* pMaterial = minirender::loadMaterial();

        vertexBuffer = createBufferFromData(device, mesh->vertexData, mesh->vertexData.size(),
                                            wgpu::BufferUsage::Vertex);
        indexBuffer =
            createBufferFromData(device, mesh->indexData, mesh->indexData.size(), wgpu::BufferUsage::Index);

        material = MaterialManager::get()->getMaterialInstance(*pMaterial, mesh->vertexFormat);

        pMaterialData = &material.getUniformData();
        pMaterialData->setData(lemon::sid("packetParams.matModel"), tModel.getMatrix());

        pSharedData = &PipelineManager::get()->getSharedUniformData();
        pSharedData->setData(lemon::sid("sceneParams.camera"), camera.getUniformData());
        pSharedData->setData(lemon::sid("sceneParams.time"), 1.f);
    }

    void
    render()
    {
        auto* pPipelineMan = PipelineManager::get();
        auto* pRenderMan = RenderManager::get();

        auto& cbuffer = pRenderMan->getConstantBuffer();
        cbuffer.reset();

        pSharedData->merge(cbuffer);
        pMaterialData->merge(cbuffer);

        cbuffer.upload(pRenderMan->getDevice());

        wgpu::TextureView backbufferView = swapChain.GetCurrentTextureView();
        ComboRenderPassDescriptor renderPass({backbufferView}, depthStencilView);

        wgpu::CommandEncoder encoder = device.CreateCommandEncoder();
        {
            wgpu::RenderPassEncoder pass = encoder.BeginRenderPass(&renderPass);
            pass.SetPipeline(material.getRenderPipeline().getColorPipeline());

            pass.SetBindGroup(kSharedBindGroupIndex, pPipelineMan->getSharedBindGroup(),
                              pSharedData->getOffsetCount(), pSharedData->getOffsets());

            pass.SetBindGroup(kMaterialBindGroupIndex, material.getBindGroup(),
                              pMaterialData->getOffsetCount(), pMaterialData->getOffsets());

            pass.SetVertexBuffer(0, vertexBuffer);
            pass.SetIndexBuffer(indexBuffer, mesh->indexFormat);
            pass.DrawIndexed(mesh->indexCount);
            pass.EndPass();
        }

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
            render.render();
            return LoopControl::Continue;
        });
    }

    engine.shutdown();
}
