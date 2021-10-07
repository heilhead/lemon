#include <lemon/render/material/MaterialManager.h>
#include <lemon/render/RenderManager.h>
#include <lemon/render/utils.h>
#include <lemon/resource/types/MaterialResource.h>
#include <lemon/resource/types/TextureResource.h>
#include <lemon/shared/assert.h>

using namespace lemon::res;
using namespace lemon::res::material;
using namespace lemon::render;
using namespace lemon::shader;
using namespace lemon;
using namespace magic_enum::bitwise_operators;

static MaterialManager* gInstance;

MaterialManager::MaterialManager()
{
    LEMON_ASSERT(gInstance == nullptr);
    gInstance = this;
}

MaterialManager::~MaterialManager()
{
    gInstance = nullptr;
}

MaterialManager*
MaterialManager::get()
{
    return gInstance;
}

uint64_t
computeMaterialHash(const std::optional<MaterialBlueprint>& blueprint, const MaterialConfiguration& config)
{
    lemon::Hash hash;

    hash.append(config);

    if (blueprint) {
        hash.appendHash(blueprint->getHash());
    }

    return hash;
}

ResourceRef<ShaderProgram>
MaterialManager::getShader(const MaterialResource& material, const MaterialConfiguration& config)
{
    auto& blueprint = material.getBlueprint();
    if (!blueprint) {
        return nullptr;
    }

    // Copy config.
    MaterialConfiguration finalConfig = material.getConfig();
    finalConfig.merge(config);
    uint64_t hash = computeMaterialHash(blueprint, finalConfig);

    return std::move(shaderProgramCache.get(hash, [&]() {
        // TODO: Handle compilation errors, e.g. `template variable not found`
        auto sourceCode = blueprint->renderShaderSource(finalConfig);
        return shaderCompiler.compile(hash, sourceCode).release();
    }));
}

wgpu::ShaderStage
convertShaderStage(PipelineStage value)
{
    wgpu::ShaderStage result = wgpu::ShaderStage::None;

    if ((bool)(value & PipelineStage::kVertex)) {
        result |= wgpu::ShaderStage::Vertex;
    }

    if ((bool)(value & PipelineStage::kFragment)) {
        result |= wgpu::ShaderStage::Fragment;
    }

    if ((bool)(value & PipelineStage::kCompute)) {
        result |= wgpu::ShaderStage::Compute;
    }

    return result;
}

wgpu::TextureSampleType
convertSampleType(SampledKind value)
{
    switch (value) {
    case SampledKind::kFloat:
        return wgpu::TextureSampleType::Float;
    case SampledKind::kSInt:
        return wgpu::TextureSampleType::Sint;
    case SampledKind::kUInt:
        return wgpu::TextureSampleType::Uint;
    default:
        return wgpu::TextureSampleType::Undefined;
    }
}

wgpu::TextureViewDimension
convertViewDimension(TextureDimension value)
{
    switch (value) {
    case TextureDimension::k1d:
        return wgpu::TextureViewDimension::e1D;
    case TextureDimension::k2d:
        return wgpu::TextureViewDimension::e2D;
    case TextureDimension::k2dArray:
        return wgpu::TextureViewDimension::e2DArray;
    case TextureDimension::k3d:
        return wgpu::TextureViewDimension::e3D;
    case TextureDimension::kCube:
        return wgpu::TextureViewDimension::Cube;
    case TextureDimension::kCubeArray:
        return wgpu::TextureViewDimension::CubeArray;
    default:
        return wgpu::TextureViewDimension::Undefined;
    }
}

wgpu::BindGroupLayout
createBindGroupLayout(const MaterialResource& material, const ShaderProgram& program, uint8_t bindGroupIndex)
{
    folly::small_vector<wgpu::BindGroupLayoutEntry, 8> entries;

    for (auto& resDesc : program.getReflection()) {
        if (resDesc.bindGroup != bindGroupIndex) {
            continue;
        }

        wgpu::BindGroupLayoutEntry entry;
        entry.binding = resDesc.binding;
        entry.visibility = convertShaderStage(resDesc.stage);

        switch (resDesc.resourceType) {
        case ResourceType::kUniformBuffer:
            entry.buffer.type = wgpu::BufferBindingType::Uniform;
            entry.buffer.hasDynamicOffset = true;
            entry.buffer.minBindingSize = resDesc.size;
            break;
        case ResourceType::kSampledTexture:
            entry.texture.sampleType = convertSampleType(resDesc.sampledKind);
            entry.texture.viewDimension = convertViewDimension(resDesc.dim);
            entry.texture.multisampled = false;
            break;
        case ResourceType::kSampler:
            // TODO: Use `SamplerDescriptor` to figure out sampling parameters.
            entry.sampler.type = wgpu::SamplerBindingType::Filtering;
            break;
        default:
            LEMON_TODO();
        }

        entries.emplace_back(entry);
    }

    wgpu::BindGroupLayoutDescriptor desc;
    desc.entryCount = entries.size();
    desc.entries = entries.data();

    return RenderManager::get()->getDevice().CreateBindGroupLayout(&desc);
}

ResourceRef<wgpu::BindGroupLayout>
MaterialManager::getBindGroupLayout(const MaterialResource& material, const ShaderProgram& program)
{
    if (!program) {
        return nullptr;
    }

    return std::move(bindGroupLayoutCache.get(program.reflectionHash, [&]() {
        return new wgpu::BindGroupLayout(
            std::move(createBindGroupLayout(material, program, kUserBindGroupIndex)));
    }));
}

wgpu::Texture
createTexture(const TextureResource& textureRes)
{
    auto& imgData = textureRes.getImageData();

    wgpu::TextureDescriptor descriptor;
    descriptor.dimension = wgpu::TextureDimension::e2D;
    descriptor.size.width = imgData.width;
    descriptor.size.height = imgData.height;
    descriptor.size.depthOrArrayLayers = 1;
    descriptor.sampleCount = 1;
    descriptor.format = textureRes.getGPUFormat();
    descriptor.mipLevelCount = 1;
    descriptor.usage = wgpu::TextureUsage::CopyDst | wgpu::TextureUsage::TextureBinding;

    auto& device = RenderManager::get()->getDevice();

    auto texture = device.CreateTexture(&descriptor);

    wgpu::Buffer stagingBuffer = createBufferFromData(
        device, imgData.data, static_cast<uint32_t>(imgData.data.size()), wgpu::BufferUsage::CopySrc);
    wgpu::ImageCopyBuffer imageCopyBuffer =
        createImageCopyBuffer(stagingBuffer, 0, imgData.stride * imgData.width);
    wgpu::ImageCopyTexture imageCopyTexture = createImageCopyTexture(texture, 0, {0, 0, 0});
    wgpu::Extent3D copySize = {imgData.width, imgData.height, 1};

    wgpu::CommandEncoder encoder = device.CreateCommandEncoder();
    encoder.CopyBufferToTexture(&imageCopyBuffer, &imageCopyTexture, &copySize);

    wgpu::CommandBuffer copy = encoder.Finish();
    device.GetQueue().Submit(1, &copy);

    return texture;
}

ResourceRef<wgpu::Texture>
MaterialManager::getTexture(const TextureResource& texture)
{
    uint64_t id = 0;

    return std::move(
        textureCache.get(0, [&]() { return new wgpu::Texture(std::move(createTexture(texture))); }));
}