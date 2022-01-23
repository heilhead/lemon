#include "PostProcessRenderPass.h"
#include <lemon/device/Device.h>
#include <lemon/game/actor/GameWorld.h>

using namespace lemon;
using namespace lemon::render;
using namespace lemon::device;

struct QuadVertex {
    glm::f32vec3 pos;
    glm::u16vec2 uv0;

    constexpr QuadVertex(glm::f32vec3 pos, glm::u16vec2 uv0) noexcept : pos{pos}, uv0{uv0} {}
};

static constexpr std::array<QuadVertex, 4> gQuadVertexData = {
    QuadVertex(glm::f32vec3(-1.f, -1.f, 0.f), math::fpack<uint16_t>(glm::f32vec2(0.f, 1.f))),
    QuadVertex(glm::f32vec3(1.f, -1.f, 0.f), math::fpack<uint16_t>(glm::f32vec2(1.f, 1.f))),
    QuadVertex(glm::f32vec3(1.f, 1.f, 0.f), math::fpack<uint16_t>(glm::f32vec2(1.f, 0.f))),
    QuadVertex(glm::f32vec3(-1.f, 1.f, 0.f), math::fpack<uint16_t>(glm::f32vec2(0.f, 0.f))),
};

static constexpr std::array<uint16_t, 6> gQuadIndexData = {0, 1, 3, 3, 1, 2};
wgpu::Texture
createBloomMipTexture(uint32_t width, uint32_t height)
{
    wgpu::TextureDescriptor descriptor;
    descriptor.dimension = wgpu::TextureDimension::e2D;
    descriptor.size.depthOrArrayLayers = 1;
    descriptor.size.width = width;
    descriptor.size.height = height;
    descriptor.sampleCount = 1;
    descriptor.format = wgpu::TextureFormat::RGBA16Float;
    descriptor.mipLevelCount = 1;
    descriptor.usage = wgpu::TextureUsage::TextureBinding | wgpu::TextureUsage::RenderAttachment;

    return RenderManager::get()->getDevice().CreateTexture(&descriptor);
}

BloomPipeline::BloomPipeline(const DynamicMaterialSharedResources& matShared)
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
    target.format = wgpu::TextureFormat::RGBA16Float;
    target.blend = &blendState;
    target.writeMask = wgpu::ColorWriteMask::All;

    // Fragment state.
    wgpu::FragmentState fragmentState;
    fragmentState.targetCount = 1;
    fragmentState.targets = &target;
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

    // Create the pipelines.
    fragmentState.entryPoint = "FSPrefilterMain";
    prefilter = device.CreateRenderPipeline(&desc);

    fragmentState.entryPoint = "FSBlurHMain";
    hblur = device.CreateRenderPipeline(&desc);

    fragmentState.entryPoint = "FSBlurVMain";
    vblur = device.CreateRenderPipeline(&desc);

    fragmentState.entryPoint = "FSUpsampleMain";
    upsample = device.CreateRenderPipeline(&desc);
}

MeshGPUBuffer
createQuadGPUBuffer()
{
    auto& device = RenderManager::get()->getDevice();
    MeshGPUBuffer data;

    data.vertexBuffer = createBufferFromData(device, gQuadVertexData.data(), sizeof(gQuadVertexData),
                                             wgpu::BufferUsage::Vertex);
    data.indexBuffer =
        createBufferFromData(device, gQuadIndexData.data(), sizeof(gQuadIndexData), wgpu::BufferUsage::Index);

    data.indexCount = gQuadIndexData.size();
    data.indexFormat = wgpu::IndexFormat::Uint16;

    return data;
}

PostProcessRenderPass::PostProcessRenderPass(const res::MaterialResource* pPostProcessMaterial,
                                             const res::MaterialResource* pBloomMaterial)
    : passDesc{}, colorAttachments{}, quadBuffer{createQuadGPUBuffer()}
{
    auto* pRenderMan = RenderManager::get();
    auto* pMaterialMan = MaterialManager::get();
    auto& device = pRenderMan->getDevice();

    // This creates the prefilter bind group.
    pRenderMan->createFrameResources<DynamicMaterialInstance>(bloomMaterialResources, [&](auto& res, auto) {
        DynamicMaterialResourceDescriptor desc;
        desc.textures.emplace_back(std::make_pair(lemon::sid("tSrc"), res.colorTargetView));
        desc.textures.emplace_back(std::make_pair(lemon::sid("tSrcLow"), res.colorTargetView));
        return pMaterialMan->getDynamicMaterialInstance<BloomPipeline>(*pBloomMaterial, desc);
    });

    {
        wgpu::TextureViewDescriptor viewDesc;

        // Create bloom target textures.
        auto [width, height] = pRenderMan->getRenderTargetSize();

        auto prefilterTargetWidth = width / 2;
        auto prefilterTargetHeight = height / 2;
        bloomPrefilterTarget =
            createBloomMipTexture(prefilterTargetWidth, prefilterTargetHeight).CreateView(&viewDesc);

        for (int32_t i = 0; i < kBloomMipLevels; i++) {
            width /= 2;
            height /= 2;

            auto& mip = bloomMips[i];
            mip.txDownsample = createBloomMipTexture(width, height).CreateView(&viewDesc);
            mip.txUpsample = createBloomMipTexture(width, height).CreateView(&viewDesc);
            mip.width = width;
            mip.height = height;
        }

        const auto& sharedResources = bloomMaterialResources.getResources(0).getSharedResource();
        const auto& kaLayout = sharedResources->kaLayout;
        const auto& bgLayout = kaLayout->bindGroupLayout;
        const auto& uniformLayout = kaLayout->uniformLayout;

        BindingInitializationHelper sharedUniform(0, pRenderMan->getConstantBuffer().getBuffer(), 0,
                                                  uniformLayout.uniforms[0].size);
        BindingInitializationHelper sharedSampler(1, *sharedResources->kaSamplers[0]);

        auto& kaPipeline = bloomMaterialResources.getResources(0).getSharedResource()->kaPipeline;
        auto& pipeline = static_cast<const BloomPipeline&>(kaPipeline.get());

        // Prefilter.
        pRenderMan->createFrameResources<BloomStepData>(bloomPrefilterStep, [&](auto& res, auto) {
            BloomStepData step;
            step.pipeline = pipeline.prefilter;
            step.target = bloomPrefilterTarget;
            step.bindGroup = makeBindGroup(
                device, bgLayout,
                {sharedUniform, sharedSampler, {2, res.colorTargetView}, {3, res.colorTargetView}});
            step.uniformData.setLayout(kaLayout);
            step.srcTexSize = bloomMips[0].getTexSize();
            step.lowTexSize = glm::f32vec4(0.f);

            return step;
        });

        // Downsampling steps.
        auto hBlurSrc = bloomPrefilterTarget;
        for (int32_t i = 0; i < kBloomMipLevels; i++) {
            auto& mip = bloomMips[i];

            // Horizontal blur.
            {
                BloomStepData step;
                step.pipeline = pipeline.hblur;
                step.target = mip.txUpsample;
                step.bindGroup = makeBindGroup(device, bgLayout,
                                               {sharedUniform, sharedSampler, {2, hBlurSrc}, {3, hBlurSrc}});
                step.uniformData.setLayout(kaLayout);
                step.srcTexSize = mip.getTexSize();
                step.lowTexSize = glm::f32vec4(0.f);

                bloomSteps.emplace_back(std::move(step));
            }

            // Vertical blur.
            {
                BloomStepData step;
                step.pipeline = pipeline.vblur;
                step.target = mip.txDownsample;
                step.bindGroup =
                    makeBindGroup(device, bgLayout,
                                  {sharedUniform, sharedSampler, {2, mip.txUpsample}, {3, mip.txUpsample}});
                step.uniformData.setLayout(kaLayout);
                step.srcTexSize = mip.getTexSize();
                step.lowTexSize = glm::f32vec4(0.f);

                bloomSteps.emplace_back(std::move(step));
            }

            hBlurSrc = mip.txUpsample;
        }

        // Upsampling steps.
        auto prevUpsampleTarget = bloomMips[kBloomMipLevels - 1].txDownsample;
        for (int32_t i = kBloomMipLevels - 2; i >= 0; i--) {
            auto& prevMip = bloomMips[i + 1];
            auto& mip = bloomMips[i];

            BloomStepData step;
            step.pipeline = pipeline.upsample;
            step.target = mip.txUpsample;
            step.bindGroup =
                makeBindGroup(device, bgLayout,
                              {sharedUniform, sharedSampler, {2, prevUpsampleTarget}, {3, mip.txDownsample}});
            step.uniformData.setLayout(kaLayout);
            step.srcTexSize = mip.getTexSize();
            step.lowTexSize = prevMip.getTexSize();

            bloomSteps.emplace_back(std::move(step));

            prevUpsampleTarget = mip.txUpsample;
        }
    }

    auto& pipelineMan = pRenderMan->getPipelineManager();

    pRenderMan->createFrameResources<wgpu::BindGroup>(defaultBindGroup, [&](auto& res, auto) {
        return pipelineMan.createPostProcessBindGroup(res.colorTargetView);
    });

    {
        // Set up the post process material.
        DynamicMaterialResourceDescriptor desc;
        desc.textures.emplace_back(std::make_pair(lemon::sid("tBloom"), bloomMips[0].txUpsample));
        material = pMaterialMan->getDynamicMaterialInstance<PostProcessPipeline>(*pPostProcessMaterial, desc);

        auto& materialData = material.getUniformData();
        materialData.setData(lemon::sid("materialParams.bloomTexSize"), bloomMips[0].getTexSize());
    }

    passDesc.colorAttachmentCount = colorAttachments.size();
    passDesc.colorAttachments = colorAttachments.data();
    passDesc.depthStencilAttachment = nullptr;

    // Main color attachment.
    colorAttachments[0].loadOp = wgpu::LoadOp::Clear;
    colorAttachments[0].storeOp = wgpu::StoreOp::Store;

    // Bloom defaults.
    bloomParams.threshold = 0.42f;
    bloomParams.scatter = 0.6305f;
    bloomParams.clampMax = 25000.f;
}

lemon::render::PostProcessRenderPass::~PostProcessRenderPass() {}

void
PostProcessRenderPass::prepare(const RenderPassContext& context)
{
    auto* pRenderMan = RenderManager::get();
    auto& cbuffer = pRenderMan->getConstantBuffer();

    // Post-process & bloom shaders shared bind group (0) data.
    auto& sharedData = pRenderMan->getPipelineManager().getPostProcessUniformData();
    sharedData.merge(cbuffer);

    // Post-process shader material bind group (1) data.
    auto& materialData = material.getUniformData();
    materialData.merge(cbuffer);

    bloomParams.thresholdKnee = bloomParams.threshold / 2.f;

    // Bloom shader material bind group (1) data. Unique to each bloom step.
    setBloomStepUniformData(bloomPrefilterStep.getResources(context));

    for (auto& step : bloomSteps) {
        setBloomStepUniformData(step);
    }
}

VoidTask<RenderPassError>
PostProcessRenderPass::execute(const RenderPassContext& context,
                               std::vector<wgpu::CommandBuffer>& commandBuffers)
{
    colorAttachments[0].view = context.pCurrentFrame->swapChainBackbufferView;

    auto* pRenderMan = RenderManager::get();
    auto* pPipelineMan = PipelineManager::get();

    const auto& sharedData = pPipelineMan->getPostProcessUniformData();
    const auto& sharedBindGroup = defaultBindGroup.getResources(context);

    wgpu::CommandEncoder encoder = pRenderMan->getDevice().CreateCommandEncoder();

    {
        auto executeBloomStep = [&](const BloomStepData& step) {
            wgpu::RenderPassColorAttachment target;
            target.view = step.target;
            target.loadOp = wgpu::LoadOp::Clear;
            target.storeOp = wgpu::StoreOp::Store;
            target.clearColor = wgpu::Color{.r = 0.f, .g = 0.f, .b = 0.f, .a = 1.f};

            wgpu::RenderPassDescriptor prefilterPassDesc;
            prefilterPassDesc.colorAttachmentCount = 1;
            prefilterPassDesc.colorAttachments = &target;

            wgpu::RenderPassEncoder pass = encoder.BeginRenderPass(&prefilterPassDesc);

            pass.SetPipeline(step.pipeline);

            pass.SetBindGroup(kPostProcessSharedBindGroupIndex, sharedBindGroup, sharedData.getOffsetCount(),
                              sharedData.getOffsets());

            pass.SetBindGroup(kMaterialBindGroupIndex, step.bindGroup, step.uniformData.getOffsetCount(),
                              step.uniformData.getOffsets());

            pass.SetVertexBuffer(0, quadBuffer.vertexBuffer);
            pass.SetIndexBuffer(quadBuffer.indexBuffer, quadBuffer.indexFormat);
            pass.DrawIndexed(quadBuffer.indexCount);

            pass.EndPass();
        };

        executeBloomStep(bloomPrefilterStep.getResources(context));

        for (const auto& step : bloomSteps) {
            executeBloomStep(step);
        }
    }

    {
        auto& materialData = material.getUniformData();

        wgpu::RenderPassEncoder pass = encoder.BeginRenderPass(&passDesc);

        const auto& kaPipeline = material.getSharedResource()->kaPipeline;
        const auto& pipeline = static_cast<const PostProcessPipeline&>(*kaPipeline);

        pass.SetPipeline(pipeline.getMainPipeline());

        pass.SetBindGroup(kPostProcessSharedBindGroupIndex, sharedBindGroup, sharedData.getOffsetCount(),
                          sharedData.getOffsets());

        pass.SetBindGroup(kMaterialBindGroupIndex, material.getBindGroup(), materialData.getOffsetCount(),
                          materialData.getOffsets());

        pass.SetVertexBuffer(0, quadBuffer.vertexBuffer);
        pass.SetIndexBuffer(quadBuffer.indexBuffer, quadBuffer.indexFormat);
        pass.DrawIndexed(quadBuffer.indexCount);

        pass.EndPass();
    }

    commandBuffers.emplace_back(encoder.Finish());

    co_return {};
}

void
PostProcessRenderPass::setBloomStepUniformData(BloomStepData& step)
{
    bloomParams.srcTexSize = step.srcTexSize;
    bloomParams.lowTexSize = step.lowTexSize;

    constexpr auto id = lemon::sid("bloomParams");

    auto& uniform = step.uniformData;
    uniform.setData(id, bloomParams);
    uniform.merge(RenderManager::get()->getConstantBuffer());
}
