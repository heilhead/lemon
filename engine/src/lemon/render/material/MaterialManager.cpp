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

void
MaterialManager::releaseResources()
{
    shaderProgramCache.clear();
    materialLayoutCache.clear();
    samplerCache.clear();
    textureCache.clear();
    surfaceSharedResourcesCache.clear();
    dynamicSharedResourcesCache.clear();
}

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

    return getShader(*blueprint, finalConfig);
}

KeepAlive<ShaderProgram>
MaterialManager::getShader(const MaterialBlueprint& blueprint,
                           const render::MaterialConfiguration& finalConfig)
{
    uint64_t hash = computeMaterialHash(blueprint, finalConfig);

    return shaderProgramCache.get(hash, [&]() {
        // TODO: Handle compilation errors, e.g. `template variable not found`
        auto sourceCode = blueprint.renderShaderSource(finalConfig);
        return shaderCompiler.compile(hash, sourceCode).release();
    });
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

    return materialLayoutCache.get(id,
                                   [&]() { return new MaterialLayout(material, program, bindGroupIndex); });
}

KeepAlive<MaterialLayout>
MaterialManager::getMaterialLayout(const ShaderProgram& program, uint8_t bindGroupIndex)
{
    if (!program) {
        return nullptr;
    }

    auto id = computeMaterialLayoutID(program, bindGroupIndex);

    return materialLayoutCache.get(id, [&]() { return new MaterialLayout(program, bindGroupIndex); });
}

SurfaceMaterialInstance
MaterialManager::getSurfaceMaterialInstance(const MaterialResource& material,
                                            const MeshVertexFormat& vertexFormat)
{
    LEMON_ASSERT(material.getDomainDescriptor().type == MaterialResource::Domain::Surface);

    const MaterialResourceDescriptor desc{.pResource = &material,
                                          .meshComponents = vertexFormat.getComponents()};
    const auto id = lemon::hash(desc);
    const auto kaSharedResources = surfaceSharedResourcesCache.get(id, [&]() {
        auto* pMatShared = new SurfaceMaterialSharedResources(id, material, vertexFormat);
        pMatShared->kaPipeline = PipelineManager::get()->getSurfacePipeline(*pMatShared, vertexFormat);
        return pMatShared;
    });

    return SurfaceMaterialInstance(kaSharedResources);
}

// PostProcessMaterialInstance
// MaterialManager::getPostProcessMaterialInstance(const res::MaterialResource& material)
//{
//     LEMON_ASSERT(material.getDomainDescriptor().type == MaterialResource::Domain::PostProcess);
//
//     const MaterialResourceDescriptor desc{.pResource = &material, .meshComponents = MeshComponents::None};
//     const auto id = lemon::hash(desc);
//     const auto kaSharedResources = postProcessSharedResourcesCache.get(id, [&]() {
//         auto* pMatShared = new PostProcessMaterialSharedResources(id, material);
//         pMatShared->kaPipeline = PipelineManager::get()->getPostProcessPipeline(*pMatShared);
//         return pMatShared;
//     });
//
//     return PostProcessMaterialInstance(kaSharedResources);
// }

wgpu::Texture
MaterialManager::createTexture(const TextureResource& textureRes)
{
    wgpu::TextureDescriptor descriptor;
    descriptor.dimension = wgpu::TextureDimension::e2D;
    descriptor.size.depthOrArrayLayers = 1;
    descriptor.sampleCount = 1;
    descriptor.format = textureRes.getGPUFormat();
    descriptor.mipLevelCount = textureRes.getMipLevelCount();
    descriptor.usage = wgpu::TextureUsage::CopyDst | wgpu::TextureUsage::TextureBinding;

    auto& imgData = textureRes.getImageData();

    if (textureRes.isRenderTarget()) {
        descriptor.usage |= wgpu::TextureUsage::RenderAttachment;

        auto [width, height] = textureRes.getRenderTargetDimensions();

        descriptor.size.width = width;
        descriptor.size.height = height;
    } else {
        descriptor.size.width = imgData.width;
        descriptor.size.height = imgData.height;
    }

    auto texture = pDevice->CreateTexture(&descriptor);

    if (textureRes.hasImageData()) {
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
    }

    return texture;
}

KeepAlive<wgpu::Texture>
MaterialManager::getTexture(const TextureResource& texture)
{
    uint64_t id = lemon::hash(texture);

    return textureCache.get(id, [&]() { return new wgpu::Texture(createTexture(texture)); });
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

    return getSampler(desc);
}

KeepAlive<wgpu::Sampler>
MaterialManager::getSampler(const wgpu::SamplerDescriptor& desc)
{
    const auto id = lemon::hash(desc);
    return samplerCache.get(id, [&]() { return new wgpu::Sampler(pDevice->CreateSampler(&desc)); });
}

void
MaterialManager::init(wgpu::Device& device)
{
    pDevice = &device;
}

uint64_t
MaterialManager::calculateDynamicMaterialHash(const res::MaterialResource& material,
                                              const DynamicMaterialResourceDescriptor& dynamicBindings,
                                              const MaterialConfiguration* pConfig)
{
    LEMON_ASSERT(material.getDomainDescriptor().type == MaterialResource::Domain::Dynamic);

    Hash h(material.getHandle(), dynamicBindings);

    if (pConfig) {
        h.append(*pConfig);
    }

    return h.value();
}
