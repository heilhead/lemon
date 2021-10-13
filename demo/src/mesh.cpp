#include "mesh.h"

#include "ozz/base/io/stream.h"
#include "ozz/base/io/archive.h"
#include "ozz/animation/runtime/skeleton.h"
#include "ozz/animation/runtime/animation.h"

#include <lemon/shared/utils.h>
#include <lemon/shared/logger.h>
#include <lemon/engine.h>
#include <lemon/device/Device.h>
#include <lemon/shared/filesystem.h>
#include <lemon/resources.h>
#include <lemon/resource/types/ModelResource.h>
#include <lemon/render/ComboRenderPassDescriptor.h>
#include <lemon/render/utils.h>
#include <lemon/render/RenderManager.h>
#include <lemon/render/PipelineManager.h>
#include <lemon/render/material/MaterialManager.h>

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

#include <dawn/webgpu_cpp.h>
#include <dawn/webgpu_cpp_print.h>
#include <dawn/dawn_proc.h>
#include <dawn_native/D3D12Backend.h>

using namespace lemon;
using namespace lemon::device;
using namespace lemon::utils;
using namespace lemon::res;
using namespace lemon::scheduler;
using namespace lemon::render;

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
        ResourceLocation location(R"(misc\\M_Mannequin)");

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
        glm::mat4 matIdentity;

        glm::mat4 matModel = glm::translate(matIdentity, glm::vec3(0.f, -1.f, 0.f)) *
                             glm::scale(matIdentity, glm::vec3(0.01f, 0.01f, 0.01f)) *
                             glm::rotate(matIdentity, glm::radians(-90.f), glm::vec3(1.f, 0.f, 0.f)) *
                             glm::rotate(matIdentity, glm::radians(135.f), glm::vec3(0.f, 0.f, 1.f));

        glm::mat4 matView = glm::translate(matIdentity, glm::vec3(0.f, 0.f, -1.f)) *
                            glm::rotate(matIdentity, glm::radians(180.f), glm::vec3(0.f, 1.f, 0.f));

        auto* gpu = Device::get()->getGPU();

        device = gpu->getDevice();
        swapChain = gpu->getSwapChain();
        queue = gpu->getQueue();

        const auto& swapChainImpl = gpu->getSwapChainImpl();
        auto [wndWidth, wndHeight] = window->getSize();

        glm::mat4 matProjection = glm::perspective(90.f, (float)wndWidth / (float)wndHeight, 0.f, 5000.f);
        glm::mat4 matViewProjection = matProjection * matView * matModel;

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

        pSharedData = &PipelineManager::get()->getSharedUniformData();
        pSharedData->setData(lemon::sid("sceneParams.projection"), matViewProjection);
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

    std::string assetPath(R"(C:\git\lemon\resources)");

    engine.init(assetPath);

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
