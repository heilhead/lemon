#include <lemon/render/pipeline/SurfacePipeline.h>
#include <lemon/render/RenderManager.h>
#include <lemon/render/common.h>
#include <lemon/device/Device.h>

using namespace lemon;
using namespace lemon::render;
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
    target.format = wgpu::TextureFormat::RGBA16Float;
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
