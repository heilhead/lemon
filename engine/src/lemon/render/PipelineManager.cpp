#include <lemon/render/PipelineManager.h>
#include <lemon/render/RenderManager.h>
#include <lemon/render/BindingInitializationHelper.h>
#include <lemon/render/common.h>
#include <lemon/render/material/MaterialManager.h>
#include <lemon/render/material/MaterialInstance.h>
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

MeshSurfacePipeline::MeshSurfacePipeline(const MaterialSharedResources& matShared,
                                         const MeshVertexFormat& vertexFormat)
{
    auto* pPipelineMan = PipelineManager::get();
    auto* pRenderMan = RenderManager::get();
    auto& device = pRenderMan->getDevice();

    auto bgl0 = pPipelineMan->getSharedBindGroupLayout()->bindGroupLayout;
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
MeshSurfacePipeline::createColorPipeline(const PipelineConfiguration& config)
{
    auto* pGPUDevice = Device::get()->getGPU();
    auto& swapChainImpl = pGPUDevice->getSwapChainImpl();
    auto& device = pGPUDevice->getDevice();

    wgpu::RenderPipelineDescriptor desc;

    // Vertex state.
    auto& vertexState = desc.vertex;
    vertexState.bufferCount = 1;
    vertexState.buffers = config.pVertexLayout;
    vertexState.entryPoint = "vs_main";
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
    target.format = pGPUDevice->getColorTargetFormat();
    target.blend = &blendState;
    target.writeMask = wgpu::ColorWriteMask::All;

    wgpu::FragmentState fragmentState;
    fragmentState.targetCount = 1;
    fragmentState.targets = &target;
    fragmentState.entryPoint = "fs_main";
    fragmentState.module = *config.pColorShaderModule;

    // Primitive state.
    auto& primitiveState = desc.primitive;
    primitiveState.topology = wgpu::PrimitiveTopology::TriangleList;
    primitiveState.stripIndexFormat = wgpu::IndexFormat::Undefined;
    primitiveState.frontFace = wgpu::FrontFace::CW;
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
MeshSurfacePipeline::createDepthPipeline(const PipelineConfiguration& config)
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
    initSharedBindGroup();
}

void
PipelineManager::initSharedBindGroup()
{
    auto* pMaterialMan = MaterialManager::get();
    auto* pRenderMan = RenderManager::get();
    auto* pScheduler = Scheduler::get();
    auto& cbuffer = pRenderMan->getConstantBuffer();

    auto blueprint = std::move(
        pScheduler->block(IOTask(MaterialResource::loadShaderBlueprint(kShaderSurfaceSharedGroupBlueprint)))
            .value());

    // The program is temporary and should be destroyed once we're done here.
    KeepAlive<ShaderProgram> kaProgram;

    {
        // Compose the most complete config so that we can find all possible bindings in the program
        // reflection.
        auto config = colorConfig;

        MeshVertexFormat vertexFormat(MeshComponents::Position | MeshComponents::Normal |
                                      MeshComponents::Tangent | MeshComponents::UV0 | MeshComponents::UV1 |
                                      MeshComponents::JointInfluence);

        config.merge(std::move(vertexFormat.getMeshConfig()));
        config.define(kShaderDefineMaterialLighting, true);

        kaProgram = std::move(pMaterialMan->getShader(blueprint, config));
    }

    kaSharedBindGroupLayout = std::move(pMaterialMan->getMaterialLayout(*kaProgram, kSharedBindGroupIndex));

    folly::small_vector<wgpu::BindGroupEntry, 4> entries;

    for (auto& res : kaProgram->getReflection()) {
        if (res.bindGroup != kSharedBindGroupIndex) {
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
    descriptor.layout = kaSharedBindGroupLayout->bindGroupLayout;
    descriptor.entryCount = entries.size();
    descriptor.entries = entries.data();

    sharedBindGroup = pDevice->CreateBindGroup(&descriptor);
    sharedUniformData.setLayout(kaSharedBindGroupLayout);
}

void
PipelineManager::assignPipelines(MaterialSharedResources& matShared, const MeshVertexFormat& vertexFormat)
{
    matShared.kaPipeline = std::move(getPipeline(matShared, vertexFormat));
}

KeepAlive<MeshSurfacePipeline>
PipelineManager::getPipeline(const MaterialSharedResources& matShared, const MeshVertexFormat& vertexFormat)
{
    auto id = lemon::hash(matShared.kaColorProgram->getProgramHash(),
                          matShared.kaDepthProgram->getProgramHash(), vertexFormat.getComponents());

    return std::move(
        pipelineCache.get(id, [&]() { return new MeshSurfacePipeline(matShared, vertexFormat); }));
}
