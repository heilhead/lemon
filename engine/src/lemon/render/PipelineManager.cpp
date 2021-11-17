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

SurfacePipeline::SurfacePipeline(const SurfaceMaterialSharedResources& matShared,
                                 const MeshVertexFormat& vertexFormat)
{
    auto* pPipelineMan = PipelineManager::get();
    auto* pRenderMan = RenderManager::get();
    auto& device = pRenderMan->getDevice();

    auto bgl0 = pPipelineMan->getSurfaceBindGroupLayout()->bindGroupLayout;
    auto bgl1 = matShared.kaLayout->bindGroupLayout;

    wgpu::BindGroupLayout bindGroupLayouts[2] = {bgl0, bgl1};

    wgpu::PipelineLayoutDescriptor pipelineLayoutDesc;
    pipelineLayoutDesc.bindGroupLayoutCount = 2;
    pipelineLayoutDesc.bindGroupLayouts = bindGroupLayouts;

    wgpu::VertexBufferLayout vertexLayout;
    vertexLayout.stepMode = wgpu::VertexStepMode::Vertex;
    vertexLayout.arrayStride = vertexFormat.getStride();
    vertexLayout.attributeCount = vertexFormat.getAttributeCount();
    vertexLayout.attributes = vertexFormat.getAttributes();

    PipelineConfiguration config;
    config.pPipelineLayoutDesc = &pipelineLayoutDesc;
    config.pVertexLayout = &vertexLayout;
    config.pColorShaderModule = &matShared.kaColorProgram->getModule();
    config.pDepthShaderModule = &matShared.kaDepthProgram->getModule();

    createColorPipeline(config);
    createDepthPipeline(config);
}

void
SurfacePipeline::createColorPipeline(const PipelineConfiguration& config)
{
    auto* pGPUDevice = Device::get()->getGPU();
    auto& swapChainImpl = pGPUDevice->getSwapChainImpl();
    auto& device = pGPUDevice->getDevice();

    wgpu::RenderPipelineDescriptor desc;

    // Vertex state.
    auto& vertexState = desc.vertex;
    vertexState.bufferCount = 1;
    vertexState.buffers = config.pVertexLayout;
    vertexState.entryPoint = "VSMain";
    vertexState.module = *config.pColorShaderModule;

    // Fragment state.
    wgpu::BlendComponent blendComponent;
    blendComponent.srcFactor = wgpu::BlendFactor::One;
    blendComponent.dstFactor = wgpu::BlendFactor::Zero;
    blendComponent.operation = wgpu::BlendOperation::Add;

    wgpu::BlendState blendState;
    blendState.color = blendComponent;
    blendState.alpha = blendComponent;

    wgpu::ColorTargetState target;
    // target.format = wgpu::TextureFormat::RGBA16Float;
    target.format = pGPUDevice->getColorTargetFormat();
    target.blend = &blendState;
    target.writeMask = wgpu::ColorWriteMask::All;

    wgpu::FragmentState fragmentState;
    fragmentState.targetCount = 1;
    fragmentState.targets = &target;
    fragmentState.entryPoint = "FSMain";
    fragmentState.module = *config.pColorShaderModule;

    // Primitive state.
    auto& primitiveState = desc.primitive;
    primitiveState.topology = wgpu::PrimitiveTopology::TriangleList;
    primitiveState.stripIndexFormat = wgpu::IndexFormat::Undefined;
    primitiveState.frontFace = wgpu::FrontFace::CCW;
    primitiveState.cullMode = wgpu::CullMode::Back;

    // Depth-stencil state.
    wgpu::StencilFaceState faceState;
    faceState.compare = wgpu::CompareFunction::Always;
    faceState.failOp = wgpu::StencilOperation::Keep;
    faceState.depthFailOp = wgpu::StencilOperation::Keep;
    faceState.passOp = wgpu::StencilOperation::Keep;

    wgpu::DepthStencilState stencilState;
    stencilState.stencilFront = faceState;
    stencilState.stencilBack = faceState;
    stencilState.format = wgpu::TextureFormat::Depth24PlusStencil8;
    stencilState.depthWriteEnabled = true;
    stencilState.depthCompare = wgpu::CompareFunction::Less;
    stencilState.stencilReadMask = 0xFF;
    stencilState.stencilWriteMask = 0xFF;
    stencilState.depthBias = 0;
    stencilState.depthBiasSlopeScale = 0.0;
    stencilState.depthBiasClamp = 0.0;

    // Multisample state.
    auto& multisampleState = desc.multisample;
    multisampleState.count = 1;
    multisampleState.mask = 0xFFFFFFFF;
    multisampleState.alphaToCoverageEnabled = false;

    // Final assembly.
    desc.layout = device.CreatePipelineLayout(config.pPipelineLayoutDesc);
    desc.fragment = &fragmentState;
    desc.depthStencil = &stencilState;

    color = device.CreateRenderPipeline(&desc);
}

void
SurfacePipeline::createDepthPipeline(const PipelineConfiguration& config)
{
    // TODO: Create proper depth pipeline.
    depth = color;
}

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

void
PipelineManager::assignPipelines(SurfaceMaterialSharedResources& matShared,
                                 const MeshVertexFormat& vertexFormat)
{
    matShared.kaPipeline = std::move(getPipeline(matShared, vertexFormat));
}

wgpu::BindGroup
PipelineManager::createPostProcessBindGroup(const wgpu::TextureView& colorTargetView)
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

KeepAlive<SurfacePipeline>
PipelineManager::getPipeline(const SurfaceMaterialSharedResources& matShared,
                             const MeshVertexFormat& vertexFormat)
{
    auto id = lemon::hash(matShared.kaColorProgram->getProgramHash(),
                          matShared.kaDepthProgram->getProgramHash(), vertexFormat.getComponents());

    return std::move(
        surfacePipelineCache.get(id, [&]() { return new SurfacePipeline(matShared, vertexFormat); }));
}
