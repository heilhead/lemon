#include <lemon/render/material/MaterialManager.h>
#include <lemon/render/RenderManager.h>
#include <lemon/render/utils.h>
#include <lemon/resource/types/MaterialResource.h>
#include <lemon/resource/types/TextureResource.h>

using namespace lemon::res;
using namespace lemon::res::material;
using namespace lemon::render;
using namespace lemon::shader;
using namespace lemon;
using namespace magic_enum::bitwise_operators;

MaterialManager::MaterialManager() : pDevice{nullptr} {}

uint64_t
computeMaterialHash(const MaterialBlueprint& blueprint, const render::MaterialConfiguration& config)
{
    lemon::Hash hash;
    hash.append(config);
    hash.appendHash(blueprint.getHash());
    return hash;
}

KeepAlive<ShaderProgram>
MaterialManager::getShader(const MaterialResource& material, const render::MaterialConfiguration& config)
{
    auto& blueprint = material.getBlueprint();
    if (!blueprint) {
        return nullptr;
    }

    // Copy config.
    render::MaterialConfiguration finalConfig = material.getConfig();
    finalConfig.merge(config);

    return std::move(getShader(*blueprint, finalConfig));
}

KeepAlive<ShaderProgram>
MaterialManager::getShader(const MaterialBlueprint& blueprint,
                           const render::MaterialConfiguration& finalConfig)
{
    uint64_t hash = computeMaterialHash(blueprint, finalConfig);

    return std::move(shaderProgramCache.get(hash, [&]() {
        // TODO: Handle compilation errors, e.g. `template variable not found`
        auto sourceCode = blueprint.renderShaderSource(finalConfig);
        return shaderCompiler.compile(hash, sourceCode).release();
    }));
}

StringID
computeMaterialLayoutID(const ShaderProgram& program, uint8_t bindGroupIndex)
{
    return lemon::hash(program.getReflectionHash(), bindGroupIndex);
}

KeepAlive<MaterialLayout>
MaterialManager::getMaterialLayout(const MaterialResource& material, const ShaderProgram& program,
                                   uint8_t bindGroupIndex)
{
    if (!program) {
        return nullptr;
    }

    auto id = computeMaterialLayoutID(program, bindGroupIndex);

    return std::move(
        materialLayoutCache.get(id, [&]() { return new MaterialLayout(material, program, bindGroupIndex); }));
}

KeepAlive<MaterialLayout>
MaterialManager::getMaterialLayout(const ShaderProgram& program, uint8_t bindGroupIndex)
{
    if (!program) {
        return nullptr;
    }

    auto id = computeMaterialLayoutID(program, bindGroupIndex);

    return std::move(
        materialLayoutCache.get(id, [&]() { return new MaterialLayout(program, bindGroupIndex); }));
}

MaterialInstance
MaterialManager::getMaterialInstance(const res::MaterialResource& material,
                                     const MeshVertexFormat& vertexFormat)
{
    const MaterialResourceDescriptor desc{.pResource = &material,
                                          .meshComponents = vertexFormat.getComponents()};

    auto id = lemon::hash(desc);

    auto kaSharedResources = std::move(sharedResourcesCache.get(id, [&]() {
        auto* pMatShared = new MaterialSharedResources(material, vertexFormat);

        // TODO: Sanity check `pMatShared` for validity.

        PipelineManager::get()->assignPipelines(*pMatShared, vertexFormat);

        return pMatShared;
    }));

    return MaterialInstance(kaSharedResources);
}

wgpu::Texture
MaterialManager::createTexture(const TextureResource& textureRes)
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

    auto texture = pDevice->CreateTexture(&descriptor);

    wgpu::Buffer stagingBuffer = createBufferFromData(
        *pDevice, imgData.data, static_cast<uint32_t>(imgData.data.size()), wgpu::BufferUsage::CopySrc);
    wgpu::ImageCopyBuffer imageCopyBuffer =
        createImageCopyBuffer(stagingBuffer, 0, imgData.stride * imgData.width);
    wgpu::ImageCopyTexture imageCopyTexture = createImageCopyTexture(texture, 0, {0, 0, 0});
    wgpu::Extent3D copySize = {imgData.width, imgData.height, 1};

    wgpu::CommandEncoder encoder = pDevice->CreateCommandEncoder();
    encoder.CopyBufferToTexture(&imageCopyBuffer, &imageCopyTexture, &copySize);

    wgpu::CommandBuffer copy = encoder.Finish();
    pDevice->GetQueue().Submit(1, &copy);

    return texture;
}

KeepAlive<wgpu::Texture>
MaterialManager::getTexture(const TextureResource& texture)
{
    uint64_t id = lemon::hash(texture);

    return std::move(
        textureCache.get(id, [&]() { return new wgpu::Texture(std::move(createTexture(texture))); }));
}

KeepAlive<wgpu::Sampler>
MaterialManager::getSampler(const SamplerDescriptor& inDesc)
{
    wgpu::SamplerDescriptor desc;
    desc.addressModeU = inDesc.addressModeU;
    desc.addressModeV = inDesc.addressModeV;
    desc.addressModeW = inDesc.addressModeW;
    desc.magFilter = inDesc.magFilter;
    desc.minFilter = inDesc.minFilter;
    desc.mipmapFilter = inDesc.mipmapFilter;
    desc.lodMinClamp = inDesc.lodMinClamp;
    desc.lodMaxClamp = inDesc.lodMaxClamp;
    desc.compare = inDesc.compare;
    desc.maxAnisotropy = inDesc.maxAnisotropy;

    return std::move(getSampler(desc));
}

KeepAlive<wgpu::Sampler>
MaterialManager::getSampler(const wgpu::SamplerDescriptor& desc)
{
    uint64_t id = lemon::hash(desc);

    return std::move(
        samplerCache.get(id, [&]() { return new wgpu::Sampler(std::move(pDevice->CreateSampler(&desc))); }));
}

void
MaterialManager::init(wgpu::Device& device)
{
    pDevice = &device;
}
