#include "Triangle.h"
#include <cassert>
#include <lemon/prelude.h>

using namespace lemon;

void printDeviceError(WGPUErrorType errorType, const char* message, void*) {
    const char* errorTypeName = "";
    switch (errorType) {
    case WGPUErrorType_Validation:
        errorTypeName = "Validation";
        break;
    case WGPUErrorType_OutOfMemory:
        errorTypeName = "Out of memory";
        break;
    case WGPUErrorType_Unknown:
        errorTypeName = "Unknown";
        break;
    case WGPUErrorType_DeviceLost:
        errorTypeName = "Device lost";
        break;
    default:
        assert(false);
        return;
    }

    utils::printErr("WebGPU device error: ", errorTypeName, ": ", message);
}

wgpu::TextureDataLayout CreateTextureDataLayout(uint64_t offset, uint32_t bytesPerRow, uint32_t rowsPerImage) {
    wgpu::TextureDataLayout textureDataLayout;
    textureDataLayout.offset = offset;
    textureDataLayout.bytesPerRow = bytesPerRow;
    textureDataLayout.rowsPerImage = rowsPerImage;

    return textureDataLayout;
}

wgpu::Buffer
CreateBufferFromData(const wgpu::Device& device, const void* data, uint64_t size, wgpu::BufferUsage usage) {
    wgpu::BufferDescriptor descriptor;
    descriptor.size = size;
    descriptor.usage = usage | wgpu::BufferUsage::CopyDst;
    wgpu::Buffer buffer = device.CreateBuffer(&descriptor);

    device.GetQueue().WriteBuffer(buffer, 0, data, size);
    return buffer;
}

wgpu::ImageCopyBuffer CreateImageCopyBuffer(wgpu::Buffer buffer, uint64_t offset, uint32_t bytesPerRow) {
    uint32_t rowsPerImage = wgpu::kCopyStrideUndefined;

    wgpu::ImageCopyBuffer imageCopyBuffer = {};
    imageCopyBuffer.buffer = buffer;
    imageCopyBuffer.layout = CreateTextureDataLayout(offset, bytesPerRow, rowsPerImage);

    return imageCopyBuffer;
}

wgpu::ImageCopyTexture CreateImageCopyTexture(wgpu::Texture texture, uint32_t mipLevel, wgpu::Origin3D origin) {
    wgpu::TextureAspect aspect = wgpu::TextureAspect::All;

    wgpu::ImageCopyTexture imageCopyTexture;
    imageCopyTexture.texture = texture;
    imageCopyTexture.mipLevel = mipLevel;
    imageCopyTexture.origin = origin;
    imageCopyTexture.aspect = aspect;

    return imageCopyTexture;
}

wgpu::PipelineLayout MakeBasicPipelineLayout(const wgpu::Device& device, const wgpu::BindGroupLayout* bindGroupLayout) {
    wgpu::PipelineLayoutDescriptor descriptor;
    if (bindGroupLayout != nullptr) {
        descriptor.bindGroupLayoutCount = 1;
        descriptor.bindGroupLayouts = bindGroupLayout;
    } else {
        descriptor.bindGroupLayoutCount = 0;
        descriptor.bindGroupLayouts = nullptr;
    }
    return device.CreatePipelineLayout(&descriptor);
}

wgpu::TextureView CreateDefaultDepthStencilView(const wgpu::Device& device, uint32_t width, uint32_t height) {
    wgpu::TextureDescriptor descriptor;
    descriptor.dimension = wgpu::TextureDimension::e2D;
    descriptor.size.width = width;
    descriptor.size.height = height;
    descriptor.size.depthOrArrayLayers = 1;
    descriptor.sampleCount = 1;
    descriptor.format = wgpu::TextureFormat::Depth24PlusStencil8;
    descriptor.mipLevelCount = 1;
    descriptor.usage = wgpu::TextureUsage::RenderAttachment;
    auto depthStencilTexture = device.CreateTexture(&descriptor);
    return depthStencilTexture.CreateView();
}

wgpu::ShaderModule CreateShaderModule(const wgpu::Device& device, const char* source) {
    wgpu::ShaderModuleWGSLDescriptor wgslDesc;
    wgslDesc.source = source;
    wgpu::ShaderModuleDescriptor descriptor;
    descriptor.nextInChain = &wgslDesc;
    return device.CreateShaderModule(&descriptor);
}

BindingLayoutEntryInitializationHelper::BindingLayoutEntryInitializationHelper(uint32_t entryBinding,
    wgpu::ShaderStage entryVisibility, wgpu::BufferBindingType bufferType, bool bufferHasDynamicOffset,
    uint64_t bufferMinBindingSize) {
    binding = entryBinding;
    visibility = entryVisibility;
    buffer.type = bufferType;
    buffer.hasDynamicOffset = bufferHasDynamicOffset;
    buffer.minBindingSize = bufferMinBindingSize;
}

BindingLayoutEntryInitializationHelper::BindingLayoutEntryInitializationHelper(uint32_t entryBinding,
    wgpu::ShaderStage entryVisibility, wgpu::SamplerBindingType samplerType) {
    binding = entryBinding;
    visibility = entryVisibility;
    sampler.type = samplerType;
}

BindingLayoutEntryInitializationHelper::BindingLayoutEntryInitializationHelper(uint32_t entryBinding,
    wgpu::ShaderStage entryVisibility, wgpu::TextureSampleType textureSampleType,
    wgpu::TextureViewDimension textureViewDimension, bool textureMultisampled) {
    binding = entryBinding;
    visibility = entryVisibility;
    texture.sampleType = textureSampleType;
    texture.viewDimension = textureViewDimension;
    texture.multisampled = textureMultisampled;
}

BindingLayoutEntryInitializationHelper::BindingLayoutEntryInitializationHelper(uint32_t entryBinding,
    wgpu::ShaderStage entryVisibility, wgpu::StorageTextureAccess storageTextureAccess, wgpu::TextureFormat format,
    wgpu::TextureViewDimension textureViewDimension) {
    binding = entryBinding;
    visibility = entryVisibility;
    storageTexture.access = storageTextureAccess;
    storageTexture.format = format;
    storageTexture.viewDimension = textureViewDimension;
}

// ExternalTextureBindingLayout never contains data, so just make one that can be reused instead
// of declaring a new one every time it's needed.
wgpu::ExternalTextureBindingLayout kExternalTextureBindingLayout = {};

BindingLayoutEntryInitializationHelper::BindingLayoutEntryInitializationHelper(uint32_t entryBinding,
    wgpu::ShaderStage entryVisibility, wgpu::ExternalTextureBindingLayout* bindingLayout) {
    binding = entryBinding;
    visibility = entryVisibility;
    nextInChain = bindingLayout;
}

BindingLayoutEntryInitializationHelper::BindingLayoutEntryInitializationHelper(const wgpu::BindGroupLayoutEntry& entry)
    :wgpu::BindGroupLayoutEntry(entry) {
}

BindingInitializationHelper::BindingInitializationHelper(uint32_t binding, const wgpu::Sampler& sampler)
    :binding(binding), sampler(sampler) {
}

BindingInitializationHelper::BindingInitializationHelper(uint32_t binding, const wgpu::TextureView& textureView)
    :binding(binding), textureView(textureView) {
}

BindingInitializationHelper::BindingInitializationHelper(uint32_t binding, const wgpu::ExternalTexture& externalTexture)
    :binding(binding) {
    externalTextureBindingEntry.externalTexture = externalTexture;
}

BindingInitializationHelper::BindingInitializationHelper(uint32_t binding, const wgpu::Buffer& buffer, uint64_t offset,
    uint64_t size)
    :binding(binding), buffer(buffer), offset(offset), size(size) {
}

wgpu::BindGroupEntry BindingInitializationHelper::GetAsBinding() const {
    wgpu::BindGroupEntry result;

    result.binding = binding;
    result.sampler = sampler;
    result.textureView = textureView;
    result.buffer = buffer;
    result.offset = offset;
    result.size = size;
    if (externalTextureBindingEntry.externalTexture != nullptr) {
        result.nextInChain = &externalTextureBindingEntry;
    }

    return result;
}

wgpu::BindGroupLayout MakeBindGroupLayout(const wgpu::Device& device,
    std::initializer_list<BindingLayoutEntryInitializationHelper> entriesInitializer) {
    std::vector<wgpu::BindGroupLayoutEntry> entries;
    for (const BindingLayoutEntryInitializationHelper& entry: entriesInitializer) {
        entries.push_back(entry);
    }

    wgpu::BindGroupLayoutDescriptor descriptor;
    descriptor.entryCount = static_cast<uint32_t>(entries.size());
    descriptor.entries = entries.data();
    return device.CreateBindGroupLayout(&descriptor);
}

wgpu::BindGroup MakeBindGroup(const wgpu::Device& device, const wgpu::BindGroupLayout& layout,
    std::initializer_list<BindingInitializationHelper> entriesInitializer) {
    std::vector<wgpu::BindGroupEntry> entries;
    for (const BindingInitializationHelper& helper: entriesInitializer) {
        entries.push_back(helper.GetAsBinding());
    }

    wgpu::BindGroupDescriptor descriptor;
    descriptor.layout = layout;
    descriptor.entryCount = (uint32_t)entries.size();
    descriptor.entries = entries.data();

    return device.CreateBindGroup(&descriptor);
}

ComboRenderPipelineDescriptor::ComboRenderPipelineDescriptor() {
    auto* descriptor = this;

    // Set defaults for the vertex state.
    {
        wgpu::VertexState* vert = &descriptor->vertex;
        vert->module = nullptr;
        vert->entryPoint = "main";
        vert->bufferCount = 0;

        // Fill the default values for vertexBuffers and vertexAttributes in buffers.
        for (uint32_t i = 0; i < kMaxVertexAttributes; ++i) {
            cAttributes[i].shaderLocation = 0;
            cAttributes[i].offset = 0;
            cAttributes[i].format = wgpu::VertexFormat::Float32;
        }
        for (uint32_t i = 0; i < kMaxVertexBuffers; ++i) {
            cBuffers[i].arrayStride = 0;
            cBuffers[i].stepMode = wgpu::VertexStepMode::Vertex;
            cBuffers[i].attributeCount = 0;
            cBuffers[i].attributes = nullptr;
        }
        // cBuffers[i].attributes points to somewhere in cAttributes.
        // cBuffers[0].attributes points to &cAttributes[0] by default. Assuming
        // cBuffers[0] has two attributes, then cBuffers[1].attributes should point to
        // &cAttributes[2]. Likewise, if cBuffers[1] has 3 attributes, then
        // cBuffers[2].attributes should point to &cAttributes[5].
        cBuffers[0].attributes = &cAttributes[0];
        vert->buffers = &cBuffers[0];
    }

    // Set the defaults for the primitive state
    {
        wgpu::PrimitiveState* prim = &descriptor->primitive;
        prim->topology = wgpu::PrimitiveTopology::TriangleList;
        prim->stripIndexFormat = wgpu::IndexFormat::Undefined;
        prim->frontFace = wgpu::FrontFace::CCW;
        prim->cullMode = wgpu::CullMode::None;
    }

    // Set the defaults for the depth-stencil state
    {
        wgpu::StencilFaceState stencilFace;
        stencilFace.compare = wgpu::CompareFunction::Always;
        stencilFace.failOp = wgpu::StencilOperation::Keep;
        stencilFace.depthFailOp = wgpu::StencilOperation::Keep;
        stencilFace.passOp = wgpu::StencilOperation::Keep;

        cDepthStencil.format = wgpu::TextureFormat::Depth24PlusStencil8;
        cDepthStencil.depthWriteEnabled = false;
        cDepthStencil.depthCompare = wgpu::CompareFunction::Always;
        cDepthStencil.stencilBack = stencilFace;
        cDepthStencil.stencilFront = stencilFace;
        cDepthStencil.stencilReadMask = 0xff;
        cDepthStencil.stencilWriteMask = 0xff;
        cDepthStencil.depthBias = 0;
        cDepthStencil.depthBiasSlopeScale = 0.0;
        cDepthStencil.depthBiasClamp = 0.0;
    }

    // Set the defaults for the multisample state
    {
        wgpu::MultisampleState* ms = &descriptor->multisample;
        ms->count = 1;
        ms->mask = 0xFFFFFFFF;
        ms->alphaToCoverageEnabled = false;
    }

    // Set the defaults for the fragment state
    {
        cFragment.module = nullptr;
        cFragment.entryPoint = "main";
        cFragment.targetCount = 1;
        cFragment.targets = &cTargets[0];
        descriptor->fragment = &cFragment;

        wgpu::BlendComponent blendComponent;
        blendComponent.srcFactor = wgpu::BlendFactor::One;
        blendComponent.dstFactor = wgpu::BlendFactor::Zero;
        blendComponent.operation = wgpu::BlendOperation::Add;

        for (uint32_t i = 0; i < kMaxColorAttachments; ++i) {
            cTargets[i].format = wgpu::TextureFormat::RGBA8Unorm;
            cTargets[i].blend = nullptr;
            cTargets[i].writeMask = wgpu::ColorWriteMask::All;

            cBlends[i].color = blendComponent;
            cBlends[i].alpha = blendComponent;
        }
    }
}

wgpu::DepthStencilState* ComboRenderPipelineDescriptor::EnableDepthStencil(wgpu::TextureFormat format) {
    this->depthStencil = &cDepthStencil;
    cDepthStencil.format = format;
    return &cDepthStencil;
}

ComboRenderPassDescriptor::ComboRenderPassDescriptor(std::initializer_list<wgpu::TextureView> colorAttachmentInfo,
    wgpu::TextureView depthStencil) {
    for (uint32_t i = 0; i < kMaxColorAttachments; ++i) {
        cColorAttachments[i].loadOp = wgpu::LoadOp::Clear;
        cColorAttachments[i].storeOp = wgpu::StoreOp::Store;
        cColorAttachments[i].clearColor = { 0.0f, 0.0f, 0.0f, 0.0f };
    }

    cDepthStencilAttachmentInfo.clearDepth = 1.0f;
    cDepthStencilAttachmentInfo.clearStencil = 0;
    cDepthStencilAttachmentInfo.depthLoadOp = wgpu::LoadOp::Clear;
    cDepthStencilAttachmentInfo.depthStoreOp = wgpu::StoreOp::Store;
    cDepthStencilAttachmentInfo.stencilLoadOp = wgpu::LoadOp::Clear;
    cDepthStencilAttachmentInfo.stencilStoreOp = wgpu::StoreOp::Store;

    colorAttachmentCount = static_cast<uint32_t>(colorAttachmentInfo.size());
    uint32_t colorAttachmentIndex = 0;
    for (const wgpu::TextureView& colorAttachment: colorAttachmentInfo) {
        if (colorAttachment.Get() != nullptr) {
            cColorAttachments[colorAttachmentIndex].view = colorAttachment;
        }
        ++colorAttachmentIndex;
    }
    colorAttachments = cColorAttachments.data();

    if (depthStencil.Get() != nullptr) {
        cDepthStencilAttachmentInfo.view = depthStencil;
        depthStencilAttachment = &cDepthStencilAttachmentInfo;
    } else {
        depthStencilAttachment = nullptr;
    }
}

ComboRenderPassDescriptor::ComboRenderPassDescriptor(const ComboRenderPassDescriptor& other) {
    *this = other;
}

ComboRenderPassDescriptor& ComboRenderPassDescriptor::operator=(
    const ComboRenderPassDescriptor& otherRenderPass) {
    cDepthStencilAttachmentInfo = otherRenderPass.cDepthStencilAttachmentInfo;
    cColorAttachments = otherRenderPass.cColorAttachments;
    colorAttachmentCount = otherRenderPass.colorAttachmentCount;

    colorAttachments = cColorAttachments.data();

    if (otherRenderPass.depthStencilAttachment != nullptr) {
        // Assign desc.depthStencilAttachment to this->depthStencilAttachmentInfo;
        depthStencilAttachment = &cDepthStencilAttachmentInfo;
    } else {
        depthStencilAttachment = nullptr;
    }

    return *this;
}

Triangle::Triangle(const lemon::Window& window) {
    // stage 1

    auto instance = std::make_unique<dawn_native::Instance>();
    instance->DiscoverDefaultAdapters();

    dawn_native::Adapter backendAdapter;
    {
        utils::print("discovering adapters...");

        auto adapters = instance->GetAdapters();
        for (dawn_native::Adapter& adapter: adapters) {
            wgpu::AdapterProperties properties;
            adapter.GetProperties(&properties);

            utils::print("found adapter: ", properties.name);

            if (properties.backendType == wgpu::BackendType::D3D12) {
                backendAdapter = adapter;
                utils::print("using adapter: ", properties.name);
                break;
            }
        }

        assert(backendAdapter && "no suitable adapter found");
    }

    auto backendDevice = backendAdapter.CreateDevice();
    auto backendProcs = dawn_native::GetProcs();

    dawnProcSetProcs(&backendProcs);
    backendProcs.deviceSetUncapturedErrorCallback(backendDevice, printDeviceError, nullptr);
    device = wgpu::Device::Acquire(backendDevice);

    // stage 2

    queue = device.GetQueue();

    auto swapChainImpl = dawn_native::d3d12::CreateNativeSwapChainImpl(backendDevice, window.getContextHandle());
    auto[wndWidth, wndHeight] = window.getSize();

    wgpu::SwapChainDescriptor swapChainDesc;
    swapChainDesc.implementation = reinterpret_cast<uint64_t>(&swapChainImpl);
    swapChain = device.CreateSwapChain(nullptr, &swapChainDesc);
    auto textureFormat = static_cast<wgpu::TextureFormat>(dawn_native::d3d12::GetNativeSwapChainPreferredFormat(
        &swapChainImpl));
    swapChain.Configure(textureFormat, wgpu::TextureUsage::RenderAttachment, wndWidth, wndHeight);

    // stage 3

    initBuffers();
    initTextures();

    // stage 4

    wgpu::ShaderModule vsModule = CreateShaderModule(device, R"(
        [[stage(vertex)]] fn main([[location(0)]] pos : vec4<f32>)
                               -> [[builtin(position)]] vec4<f32> {
            return pos;
        })");

    wgpu::ShaderModule fsModule = CreateShaderModule(device, R"(
        [[group(0), binding(0)]] var mySampler: sampler;
        [[group(0), binding(1)]] var myTexture : texture_2d<f32>;

        [[stage(fragment)]] fn main([[builtin(position)]] FragCoord : vec4<f32>)
                                 -> [[location(0)]] vec4<f32> {
            return textureSample(myTexture, mySampler, FragCoord.xy / vec2<f32>(640.0, 480.0));
        })");

    auto bgl = MakeBindGroupLayout(
        device, {
            { 0, wgpu::ShaderStage::Fragment, wgpu::SamplerBindingType::Filtering },
            { 1, wgpu::ShaderStage::Fragment, wgpu::TextureSampleType::Float },
        });

    wgpu::PipelineLayout pl = MakeBasicPipelineLayout(device, &bgl);

    depthStencilView = CreateDefaultDepthStencilView(device, wndWidth, wndHeight);

    ComboRenderPipelineDescriptor descriptor;

    descriptor.layout = MakeBasicPipelineLayout(device, &bgl);
    descriptor.vertex.module = vsModule;
    descriptor.vertex.bufferCount = 1;
    descriptor.cBuffers[0].arrayStride = 4 * sizeof(float);
    descriptor.cBuffers[0].attributeCount = 1;
    descriptor.cAttributes[0].format = wgpu::VertexFormat::Float32x4;
    descriptor.cFragment.module = fsModule;
    descriptor.cTargets[0]
        .format = static_cast<wgpu::TextureFormat>(dawn_native::d3d12::GetNativeSwapChainPreferredFormat(
        &swapChainImpl));
    descriptor.EnableDepthStencil(wgpu::TextureFormat::Depth24PlusStencil8);

    pipeline = device.CreateRenderPipeline((const wgpu::RenderPipelineDescriptor*)&descriptor);

    wgpu::TextureView view = texture.CreateView();

    bindGroup = MakeBindGroup(device, bgl, {{ 0, sampler }, { 1, view }});
}

void Triangle::initBuffers() {
    static const uint32_t indexData[3] = {
        0,
        1,
        2,
    };

    indexBuffer = CreateBufferFromData(device, indexData, sizeof(indexData), wgpu::BufferUsage::Index);

    static const float vertexData[12] = {
        0.0f, 0.5f, 0.0f, 1.0f, -0.5f, -0.5f, 0.0f, 1.0f, 0.5f, -0.5f, 0.0f, 1.0f,
    };

    vertexBuffer = CreateBufferFromData(device, vertexData, sizeof(vertexData), wgpu::BufferUsage::Vertex);
}

void Triangle::initTextures() {
    wgpu::TextureDescriptor descriptor;
    descriptor.dimension = wgpu::TextureDimension::e2D;
    descriptor.size.width = 1024;
    descriptor.size.height = 1024;
    descriptor.size.depthOrArrayLayers = 1;
    descriptor.sampleCount = 1;
    descriptor.format = wgpu::TextureFormat::RGBA8Unorm;
    descriptor.mipLevelCount = 1;
    descriptor.usage = wgpu::TextureUsage::CopyDst | wgpu::TextureUsage::Sampled;

    texture = device.CreateTexture(&descriptor);
    sampler = device.CreateSampler();

    // Initialize the texture with arbitrary data until we can load images
    std::vector<uint8_t> data(4 * 1024 * 1024, 0);
    for (size_t i = 0; i < data.size(); ++i) {
        data[i] = static_cast<uint8_t>(i % 253);
    }

    wgpu::Buffer stagingBuffer = CreateBufferFromData(
        device, data.data(), static_cast<uint32_t>(data.size()), wgpu::BufferUsage::CopySrc);
    wgpu::ImageCopyBuffer imageCopyBuffer = CreateImageCopyBuffer(stagingBuffer, 0, 4 * 1024);
    wgpu::ImageCopyTexture imageCopyTexture = CreateImageCopyTexture(texture, 0, { 0, 0, 0 });
    wgpu::Extent3D copySize = { 1024, 1024, 1 };

    wgpu::CommandEncoder encoder = device.CreateCommandEncoder();
    encoder.CopyBufferToTexture(&imageCopyBuffer, &imageCopyTexture, &copySize);

    wgpu::CommandBuffer copy = encoder.Finish();
    queue.Submit(1, &copy);
}

void Triangle::render() {
    wgpu::TextureView backbufferView = swapChain.GetCurrentTextureView();
    ComboRenderPassDescriptor renderPass({ backbufferView }, depthStencilView);

    wgpu::CommandEncoder encoder = device.CreateCommandEncoder();
    {
        wgpu::RenderPassEncoder pass = encoder.BeginRenderPass(&renderPass);
        pass.SetPipeline(pipeline);
        pass.SetBindGroup(0, bindGroup);
        pass.SetVertexBuffer(0, vertexBuffer);
        pass.SetIndexBuffer(indexBuffer, wgpu::IndexFormat::Uint32);
        pass.DrawIndexed(3);
        pass.EndPass();
    }

    wgpu::CommandBuffer commands = encoder.Finish();
    queue.Submit(1, &commands);
    swapChain.Present();
}
