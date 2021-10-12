#include <lemon/render/PipelineManager.h>
#include <lemon/render/RenderManager.h>
#include <lemon/render/BindingInitializationHelper.h>
#include <lemon/render/common.h>
#include <lemon/render/material/MaterialManager.h>
#include <lemon/render/material/MaterialInstance.h>
#include <lemon/render/material/ShaderProgram.h>
#include <lemon/device/Device.h>
#include <lemon/shared/logger.h>
#include <folly/small_vector.h>

using namespace lemon;
using namespace lemon::render;
using namespace lemon::shader;
using namespace lemon::device;

static PipelineManager* gInstance;

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
    primitiveState.cullMode = wgpu::CullMode::Front;

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
    LEMON_ASSERT(gInstance == nullptr);
    gInstance = this;

    colorConfig.define("PIPELINE_DEPTH_ONLY", false);

    depthConfig = colorConfig;
    depthConfig.define("PIPELINE_DEPTH_ONLY", true);
}

PipelineManager::~PipelineManager()
{
    gInstance = nullptr;
}

PipelineManager*
PipelineManager::get()
{
    return gInstance;
}

void
PipelineManager::init(wgpu::Device& device)
{
    pDevice = &device;
}

void
PipelineManager::initSharedBindGroup(const KeepAlive<ShaderProgram>& kaColorProgram)
{
    // TODO: Is it actually a good idea to create the shared bind group based on an arbitrary material's color
    // program? Where does the lighting and skinning data binds?

    auto* pMaterialMan = MaterialManager::get();
    auto* pRenderMan = RenderManager::get();
    auto& cbuffer = pRenderMan->getConstantBuffer();

    kaSharedBindGroupLayout =
        std::move(pMaterialMan->getMaterialLayout(*kaColorProgram, kSharedBindGroupIndex));

    folly::small_vector<wgpu::BindGroupEntry, 4> entries;

    for (auto& res : kaColorProgram->getReflection()) {
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
PipelineManager::getPipeline(const MaterialSharedResources& matShared,
                                    const MeshVertexFormat& vertexFormat)
{
    auto id = lemon::hash(matShared.kaColorProgram->getProgramHash(),
                          matShared.kaDepthProgram->getProgramHash(), vertexFormat.getComponents());

    return std::move(pipelineCache.get(id, [&]() {
        // TODO: Find a better place to initialize the shared bind group.
        if (!isSharedBindGroupReady()) {
            initSharedBindGroup(matShared.kaColorProgram);
        }

        return new MeshSurfacePipeline(matShared, vertexFormat);
    }));
}