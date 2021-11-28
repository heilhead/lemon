#include <lemon/render/material/MaterialInstance.h>
#include <lemon/resource/ResourceManager.h>
#include <lemon/resource/types/MaterialResource.h>
#include <lemon/shader/reflection.h>
#include <lemon/render/RenderManager.h>
#include <lemon/render/BindingInitializationHelper.h>
#include <lemon/render/MeshVertexFormat.h>

using namespace lemon;
using namespace lemon::render;
using namespace lemon::res;
using namespace lemon::shader;

template<typename TData, size_t Size>
inline const TData*
findByID(const MaterialResource::ResourceList<TData, Size>& data, StringID id)
{
    for (auto& [k, v] : data) {
        if (k == id) {
            return &v;
        }
    }

    return nullptr;
}

void
MaterialSharedResources::initBindGroup(const MaterialResource& matRes, const ShaderProgram& program,
                                       const DynamicMaterialResourceDescriptor* pDynamicBindings)
{
    auto* pResourceMan = ResourceManager::get();
    auto* pRenderMan = RenderManager::get();
    auto* pMaterialMan = MaterialManager::get();
    auto& cbuffer = pRenderMan->getConstantBuffer().getBuffer();

    for (auto& [k, v] : matRes.getUniformValues()) {
        std::visit([&](const auto& val) { uniformData.setData(k, val); }, v);
    }

    std::vector<wgpu::BindGroupEntry> entries;

    for (auto& res : program.getReflection()) {
        if (res.bindGroup != kMaterialBindGroupIndex) {
            continue;
        }

        auto binding = res.binding;
        auto id = res.id;

        switch (res.resourceType) {
        case ResourceType::kUniformBuffer: {
            entries.emplace_back(createBinding(binding, cbuffer, 0, res.size));
            break;
        }

        case ResourceType::kStorageBuffer:
        case ResourceType::kReadOnlyStorageBuffer: {
            LEMON_ASSERT(pDynamicBindings != nullptr,
                         "storage buffers can only be bound using dynamic bindings");

            auto* pSearch = findByID(pDynamicBindings->buffers, id);

            LEMON_ASSERT(pSearch != nullptr, "storage buffer binding not found");

            entries.emplace_back(createBinding(binding, *pSearch));
            break;
        }

        case ResourceType::kSampler:
        case ResourceType::kComparisonSampler: {
            wgpu::Sampler sampler;

            if (pDynamicBindings != nullptr) {
                if (const auto* pSearch = findByID(pDynamicBindings->samplers, id)) {
                    sampler = *pSearch;
                }
            }

            if (!sampler) {
                const auto* search = findByID(matRes.getSamplerDescriptors(), id);

                LEMON_ASSERT(search != nullptr,
                             "failed to create bind group: binding not found for sampler. id: ", id);

                auto kaSampler = pMaterialMan->getSampler(*search);

                sampler = *kaSampler;

                // Store the keep-alive for the lifetime of this instance.
                kaSamplers.emplace_back(std::move(kaSampler));
            }

            entries.emplace_back(createBinding(binding, sampler));
            break;
        }

        case ResourceType::kSampledTexture:
        case ResourceType::kMultisampledTexture:
        case ResourceType::kDepthTexture:
        case ResourceType::kDepthMultisampledTexture: {
            wgpu::TextureView view;

            if (pDynamicBindings != nullptr) {
                if (const auto* pSearch = findByID(pDynamicBindings->textures, id)) {
                    view = *pSearch;
                }
            }

            if (!view) {
                const auto* pSearch = findByID(matRes.getTextureLocations(), id);

                LEMON_ASSERT(pSearch != nullptr,
                             "failed to create bind group: binding not found for texture. id: ", id);

                auto* pTextureRes = pResourceMan->getResource<TextureResource>(pSearch->handle);

                LEMON_ASSERT(pTextureRes != nullptr,
                             "failed to create bind group: texture resource not available. id: ", id);

                auto kaTexture = pMaterialMan->getTexture(*pTextureRes);

                view = kaTexture->CreateView();

                // Store the keep-alive for the lifetime of this instance.
                kaTextures.emplace_back(std::move(kaTexture));
            }

            entries.emplace_back(createBinding(binding, view));
            break;
        }

        case ResourceType::kReadOnlyStorageTexture:
        case ResourceType::kWriteOnlyStorageTexture: {
            LEMON_ASSERT(pDynamicBindings != nullptr,
                         "storage textures can only be bound using dynamic bindings");

            auto* pSearch = findByID(pDynamicBindings->textures, id);

            LEMON_ASSERT(pSearch != nullptr, "storage texture binding not found");

            entries.emplace_back(createBinding(binding, *pSearch));
            break;
        }

        default:
            // kExternalTexture
            LEMON_TODO();
        }
    }

    wgpu::BindGroupDescriptor descriptor;
    descriptor.layout = kaLayout->bindGroupLayout;
    descriptor.entryCount = entries.size();
    descriptor.entries = entries.data();

    bindGroup = pRenderMan->getDevice().CreateBindGroup(&descriptor);
}

SurfaceMaterialSharedResources::SurfaceMaterialSharedResources(uint64_t id, const MaterialResource& matRes,
                                                               const MeshVertexFormat& vertexFormat)
    : MaterialSharedResources(id)
{
    auto* pResourceMan = ResourceManager::get();
    auto* pRenderMan = RenderManager::get();
    auto* pMaterialMan = MaterialManager::get();

    {
        auto* pPipelineMan = PipelineManager::get();
        auto meshConfig = vertexFormat.getMeshConfig();

        // Copy default configurations.
        auto cfgMain = pPipelineMan->getSurfaceColorConfig();
        cfgMain.merge(meshConfig);

        auto cfgDepth = pPipelineMan->getSurfaceDepthConfig();
        cfgDepth.merge(meshConfig);

        kaColorProgram = pMaterialMan->getShader(matRes, cfgMain);
        kaDepthProgram = pMaterialMan->getShader(matRes, cfgDepth);

        LEMON_ASSERT(*kaColorProgram, "failed to compile shader program");
        LEMON_ASSERT(*kaDepthProgram, "failed to compile shader program");
    }

    // The layout is based on the color program, assuming that it's the most complete version.
    kaLayout = pMaterialMan->getMaterialLayout(matRes, *kaColorProgram, kMaterialBindGroupIndex);
    uniformData.setLayout(kaLayout);

    initBindGroup(matRes, *kaColorProgram);
}

PostProcessMaterialSharedResources::PostProcessMaterialSharedResources(uint64_t id,
                                                                       const MaterialResource& matRes)
    : MaterialSharedResources(id)
{
    auto* pMaterialMan = MaterialManager::get();
    auto* pPipelineMan = PipelineManager::get();

    kaMainProgram = pMaterialMan->getShader(matRes, pPipelineMan->getPostProcessConfig());
    LEMON_ASSERT(*kaMainProgram, "failed to compile shader program");

    kaLayout = pMaterialMan->getMaterialLayout(matRes, *kaMainProgram, kMaterialBindGroupIndex);
    uniformData.setLayout(kaLayout);

    initBindGroup(matRes, *kaMainProgram);
}

DynamicMaterialSharedResources::DynamicMaterialSharedResources(
    uint64_t id, const MaterialResource& matRes, const DynamicMaterialResourceDescriptor& dynamicBindings,
    const MaterialConfiguration* pAdditionalConfig)
    : MaterialSharedResources(id)
{
    auto* pResourceMan = ResourceManager::get();
    auto* pRenderMan = RenderManager::get();
    auto* pMaterialMan = MaterialManager::get();

    {
        MaterialConfiguration cfg;

        if (pAdditionalConfig) {
            cfg.merge(*pAdditionalConfig);
        }

        kaMainProgram = pMaterialMan->getShader(matRes, cfg);
        LEMON_ASSERT(*kaMainProgram, "failed to compile shader program");
    }

    kaLayout = pMaterialMan->getMaterialLayout(matRes, *kaMainProgram, kMaterialBindGroupIndex);
    uniformData.setLayout(kaLayout);

    initBindGroup(matRes, *kaMainProgram, &dynamicBindings);
}

size_t
folly::hasher<MaterialResourceDescriptor>::operator()(const MaterialResourceDescriptor& data) const
{
    lemon::Hash hash;
    hash.append(data.pResource->getHandle(), data.meshComponents);
    return hash;
}

size_t
folly::hasher<DynamicMaterialResourceDescriptor>::operator()(
    const DynamicMaterialResourceDescriptor& data) const
{
    lemon::Hash hash;

    for (auto& sampler : data.samplers) {
        hash.append(sampler.first, sampler.second.Get());
    }

    for (auto& texture : data.textures) {
        hash.append(texture.first, texture.second.Get());
    }

    for (auto& buffer : data.buffers) {
        hash.append(buffer.first, buffer.second.Get());
    }

    return hash;
}
