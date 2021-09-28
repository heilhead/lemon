#include <lemon/render/utils.h>

using namespace lemon::render;

// ExternalTextureBindingLayout never contains data, so just make one that can be reused instead
// of declaring a new one every time it's needed.
wgpu::ExternalTextureBindingLayout kExternalTextureBindingLayout = {};

wgpu::TextureDataLayout
lemon::render::createTextureDataLayout(uint64_t offset, uint32_t bytesPerRow, uint32_t rowsPerImage)
{
    wgpu::TextureDataLayout textureDataLayout;
    textureDataLayout.offset = offset;
    textureDataLayout.bytesPerRow = bytesPerRow;
    textureDataLayout.rowsPerImage = rowsPerImage;

    return textureDataLayout;
}

wgpu::Buffer
lemon::render::createBufferFromData(const wgpu::Device& device, const void* data, uint64_t size,
                                    wgpu::BufferUsage usage)
{
    wgpu::BufferDescriptor descriptor;
    descriptor.size = size;
    descriptor.usage = usage | wgpu::BufferUsage::CopyDst;
    wgpu::Buffer buffer = device.CreateBuffer(&descriptor);

    device.GetQueue().WriteBuffer(buffer, 0, data, size);
    return buffer;
}

wgpu::ImageCopyBuffer
lemon::render::createImageCopyBuffer(wgpu::Buffer buffer, uint64_t offset, uint32_t bytesPerRow,
                                     uint32_t rowsPerImage)
{
    wgpu::ImageCopyBuffer imageCopyBuffer = {};
    imageCopyBuffer.buffer = buffer;
    imageCopyBuffer.layout = createTextureDataLayout(offset, bytesPerRow, rowsPerImage);

    return imageCopyBuffer;
}

wgpu::ImageCopyTexture
lemon::render::createImageCopyTexture(wgpu::Texture texture, uint32_t mipLevel, wgpu::Origin3D origin,
                                      wgpu::TextureAspect aspect)
{
    wgpu::ImageCopyTexture imageCopyTexture;
    imageCopyTexture.texture = texture;
    imageCopyTexture.mipLevel = mipLevel;
    imageCopyTexture.origin = origin;
    imageCopyTexture.aspect = aspect;

    return imageCopyTexture;
}

wgpu::ShaderModule
lemon::render::createShaderModule(const wgpu::Device& device, const char* source)
{
    wgpu::ShaderModuleWGSLDescriptor wgslDesc;
    wgslDesc.source = source;
    wgpu::ShaderModuleDescriptor descriptor;
    descriptor.nextInChain = &wgslDesc;
    return device.CreateShaderModule(&descriptor);
}

wgpu::BindGroupLayout
lemon::render::makeBindGroupLayout(
    const wgpu::Device& device,
    std::initializer_list<BindingLayoutEntryInitializationHelper> entriesInitializer)
{
    std::vector<wgpu::BindGroupLayoutEntry> entries;
    for (const BindingLayoutEntryInitializationHelper& entry : entriesInitializer) {
        entries.push_back(entry);
    }

    wgpu::BindGroupLayoutDescriptor descriptor;
    descriptor.entryCount = static_cast<uint32_t>(entries.size());
    descriptor.entries = entries.data();
    return device.CreateBindGroupLayout(&descriptor);
}

wgpu::PipelineLayout
lemon::render::makeBasicPipelineLayout(const wgpu::Device& device,
                                       const wgpu::BindGroupLayout* bindGroupLayout)
{
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

wgpu::TextureView
lemon::render::createDefaultDepthStencilView(const wgpu::Device& device, uint32_t width, uint32_t height)
{
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

wgpu::BindGroup
lemon::render::makeBindGroup(const wgpu::Device& device, const wgpu::BindGroupLayout& layout,
                             std::initializer_list<BindingInitializationHelper> entriesInitializer)
{
    std::vector<wgpu::BindGroupEntry> entries;
    for (const BindingInitializationHelper& helper : entriesInitializer) {
        entries.push_back(helper.GetAsBinding());
    }

    wgpu::BindGroupDescriptor descriptor;
    descriptor.layout = layout;
    descriptor.entryCount = entries.size();
    descriptor.entries = entries.data();

    return device.CreateBindGroup(&descriptor);
}