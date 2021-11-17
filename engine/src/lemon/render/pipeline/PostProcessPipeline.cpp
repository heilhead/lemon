#include <lemon/render/pipeline/PostProcessPipeline.h>
#include <lemon/render/RenderManager.h>
#include <lemon/render/common.h>
#include <lemon/device/Device.h>

using namespace lemon;
using namespace lemon::render;
using namespace lemon::device;

PostProcessPipeline::PostProcessPipeline(const PostProcessMaterialSharedResources& matShared)
{
    using namespace magic_enum::bitwise_operators;

    auto* pPipelineMan = PipelineManager::get();
    auto* pRenderMan = RenderManager::get();
    auto* pGPUDevice = Device::get()->getGPU();
    auto& device = pRenderMan->getDevice();
    auto& swapChainImpl = pGPUDevice->getSwapChainImpl();

    auto bgl0 = pPipelineMan->getPostProcessBindGroupLayout()->bindGroupLayout;
    auto bgl1 = matShared.kaLayout->bindGroupLayout;

    wgpu::BindGroupLayout bindGroupLayouts[2] = {bgl0, bgl1};

    wgpu::PipelineLayoutDescriptor pipelineLayoutDesc;
    pipelineLayoutDesc.bindGroupLayoutCount = 2;
    pipelineLayoutDesc.bindGroupLayouts = bindGroupLayouts;

    MeshVertexFormat vfmt(MeshComponents::Position | MeshComponents::UV0);

    wgpu::VertexBufferLayout vertexLayout;
    vertexLayout.stepMode = wgpu::VertexStepMode::Vertex;
    vertexLayout.arrayStride = vfmt.getStride();
    vertexLayout.attributeCount = vfmt.getAttributeCount();
    vertexLayout.attributes = vfmt.getAttributes();

    wgpu::RenderPipelineDescriptor desc;

    // Vertex state.
    auto& vertexState = desc.vertex;
    vertexState.bufferCount = 1;
    vertexState.buffers = &vertexLayout;
    vertexState.entryPoint = "VSMain";
    vertexState.module = matShared.kaMainProgram->getModule();

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
    fragmentState.entryPoint = "FSMain";
    fragmentState.module = matShared.kaMainProgram->getModule();

    // Primitive state.
    auto& primitiveState = desc.primitive;
    primitiveState.topology = wgpu::PrimitiveTopology::TriangleList;
    primitiveState.stripIndexFormat = wgpu::IndexFormat::Undefined;
    primitiveState.frontFace = wgpu::FrontFace::CCW;
    primitiveState.cullMode = wgpu::CullMode::None;

    // Multisample state.
    auto& multisampleState = desc.multisample;
    multisampleState.count = 1;
    multisampleState.mask = 0xFFFFFFFF;
    multisampleState.alphaToCoverageEnabled = false;

    // Final assembly.
    desc.layout = device.CreatePipelineLayout(&pipelineLayoutDesc);
    desc.fragment = &fragmentState;
    desc.depthStencil = nullptr;

    main = device.CreateRenderPipeline(&desc);
}
