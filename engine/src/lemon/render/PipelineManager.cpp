#include <lemon/render/PipelineManager.h>
#include <lemon/render/RenderManager.h>
#include <lemon/render/BindingInitializationHelper.h>
#include <lemon/render/common.h>
#include <lemon/render/material/MaterialManager.h>
#include <lemon/render/material/ShaderProgram.h>
#include <lemon/resource/types/MaterialResource.h>
#include <lemon/scheduler.h>
#include <lemon/device/Device.h>

using namespace lemon;
using namespace lemon::res;
using namespace lemon::scheduler;
using namespace lemon::render;
using namespace lemon::shader;
using namespace lemon::device;

PipelineManager::PipelineManager() : pDevice{nullptr}
{
    colorConfig.define(kShaderDefinePipelineDepthOnly, false);

    depthConfig = colorConfig;
    depthConfig.define(kShaderDefinePipelineDepthOnly, true);
}

void
PipelineManager::init(wgpu::Device& device)
{
    pDevice = &device;
    initSurfaceBindGroup();
    initPostProcessBindGroup();
}

KeepAlive<SurfacePipeline>
PipelineManager::getSurfacePipeline(const SurfaceMaterialSharedResources& matShared,
                                    const MeshVertexFormat& vertexFormat)
{
    auto id = lemon::hash(matShared.kaColorProgram->getProgramHash(),
                          matShared.kaDepthProgram->getProgramHash(), vertexFormat.getComponents());

    return surfacePipelineCache.get(id, [&]() { return new SurfacePipeline(matShared, vertexFormat); });
}

void
PipelineManager::initSurfaceBindGroup()
{
    auto* pMaterialMan = MaterialManager::get();
    auto* pRenderMan = RenderManager::get();
    auto* pScheduler = Scheduler::get();
    auto& cbuffer = pRenderMan->getConstantBuffer();

    auto blueprint =
        pScheduler->block(IOTask(MaterialResource::loadShaderBlueprint(kShaderSurfaceSharedGroupBlueprint)))
            .value();

    // The program is temporary and should be destroyed once we're done here.
    KeepAlive<ShaderProgram> kaProgram;

    {
        // Compose the most complete config so that we can find all possible bindings in the program
        // reflection.
        auto config = colorConfig;

        MeshVertexFormat vertexFormat(MeshComponents::Position | MeshComponents::Normal |
                                      MeshComponents::Tangent | MeshComponents::UV0 | MeshComponents::UV1 |
                                      MeshComponents::JointInfluence);

        config.merge(vertexFormat.getMeshConfig());
        config.define(kShaderDefineMaterialLighting, true);

        kaProgram = pMaterialMan->getShader(blueprint, config);
    }

    kaSurfaceBindGroupLayout = pMaterialMan->getMaterialLayout(*kaProgram, kSurfaceSharedBindGroupIndex);

    folly::small_vector<wgpu::BindGroupEntry, 4> entries;

    for (auto& res : kaProgram->getReflection()) {
        if (res.bindGroup != kSurfaceSharedBindGroupIndex) {
            continue;
        }

        auto binding = res.binding;
        auto id = res.id;

        switch (res.resourceType) {
        case ResourceType::kUniformBuffer: {
            entries.emplace_back(
                BindingInitializationHelper(binding, cbuffer.getBuffer(), 0, res.size).getAsBinding());
            break;
        }

        default:
            LEMON_TODO("only uniform buffer bindings are allowed in the shared bind group");
        }
    }

    wgpu::BindGroupDescriptor descriptor;
    descriptor.layout = kaSurfaceBindGroupLayout->bindGroupLayout;
    descriptor.entryCount = entries.size();
    descriptor.entries = entries.data();

    surfaceBindGroup = pDevice->CreateBindGroup(&descriptor);
    surfaceUniformData.setLayout(kaSurfaceBindGroupLayout);
}

void
PipelineManager::initPostProcessBindGroup()
{
    auto* pMaterialMan = MaterialManager::get();
    auto* pRenderMan = RenderManager::get();
    auto* pScheduler = Scheduler::get();
    auto& cbuffer = pRenderMan->getConstantBuffer();

    auto blueprint =
        pScheduler
            ->block(IOTask(MaterialResource::loadShaderBlueprint(kShaderPostProcessSharedGroupBlueprint)))
            .value();

    // The program is temporary and should be destroyed once we're done here.
    KeepAlive<ShaderProgram> kaProgram = pMaterialMan->getShader(blueprint, postProcessConfig);

    kaPostProcessBindGroupLayout =
        pMaterialMan->getMaterialLayout(*kaProgram, kPostProcessSharedBindGroupIndex);

    postProcessUniformData.setLayout(kaPostProcessBindGroupLayout);

    wgpu::SamplerDescriptor samplerDesc;
    samplerDesc.magFilter = wgpu::FilterMode::Linear;
    postProcessColorTargetSampler = pDevice->CreateSampler(&samplerDesc);
}

wgpu::BindGroup
PipelineManager::createPostProcessBindGroup(const wgpu::TextureView& colorTargetView) const
{
    auto* pRenderMan = RenderManager::get();
    auto& cbuffer = pRenderMan->getConstantBuffer().getBuffer();
    auto& kaLayout = kaPostProcessBindGroupLayout;
    auto& uniforms = kaLayout->uniformLayout.uniforms;

    LEMON_ASSERT(kaLayout->uniformLayout.uniformCount == 2);

    std::array<wgpu::BindGroupEntry, 4> entries = {
        createBinding(0, cbuffer, 0, uniforms[0].size),
        createBinding(1, cbuffer, 0, uniforms[1].size),
        createBinding(2, postProcessColorTargetSampler),
        createBinding(3, colorTargetView),
    };

    wgpu::BindGroupDescriptor descriptor;
    descriptor.layout = kaPostProcessBindGroupLayout->bindGroupLayout;
    descriptor.entryCount = entries.size();
    descriptor.entries = entries.data();

    return pDevice->CreateBindGroup(&descriptor);
}

// KeepAlive<PostProcessPipeline>
// PipelineManager::getPostProcessPipeline(const PostProcessMaterialSharedResources& matShared)
//{
//     auto id = matShared.kaMainProgram->getProgramHash();
//     return postProcessPipelineCache.get(id, [&]() { return new PostProcessPipeline(matShared); });
// }
