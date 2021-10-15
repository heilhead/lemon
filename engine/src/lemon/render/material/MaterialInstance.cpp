#include <lemon/render/material/MaterialInstance.h>
#include <lemon/resource/ResourceManager.h>
#include <lemon/resource/types/MaterialResource.h>
#include <lemon/shader/reflection.h>
#include <lemon/shared/logger.h>
#include <lemon/render/RenderManager.h>
#include <lemon/render/BindingInitializationHelper.h>
#include <lemon/render/MeshVertexFormat.h>
#include <variant>
#include <concepts>

using namespace lemon;
using namespace lemon::render;
using namespace lemon::res;
using namespace lemon::shader;

template<typename TData>
const TData*
findByID(const std::vector<std::pair<StringID, TData>>& data, StringID id)
{
    for (auto& [k, v] : data) {
        if (k == id) {
            return &v;
        }
    }

    return nullptr;
}

MaterialSharedResources::MaterialSharedResources(const res::MaterialResource& matRes,
                                                 const MeshVertexFormat& vertexFormat)
{
    auto* pResourceMan = ResourceManager::get();
    auto* pRenderMan = RenderManager::get();
    auto* pMaterialMan = MaterialManager::get();
    auto* pPipelineMan = PipelineManager::get();
    auto& cbuffer = pRenderMan->getConstantBuffer();

    auto meshConfig = vertexFormat.getMeshConfig();

    // Copy default configurations.
    auto cfgMain = pPipelineMan->getColorConfig();
    cfgMain.merge(meshConfig);

    auto cfgDepth = pPipelineMan->getDepthConfig();
    cfgDepth.merge(meshConfig);

    kaColorProgram = pMaterialMan->getShader(matRes, cfgMain);
    kaDepthProgram = pMaterialMan->getShader(matRes, cfgMain);

    LEMON_ASSERT(*kaColorProgram, "failed to compile shader program");
    LEMON_ASSERT(*kaDepthProgram, "failed to compile shader program");

    // The layout is based on the color program, assuming that it's the most complete version.
    kaLayout = pMaterialMan->getMaterialLayout(matRes, *kaColorProgram, kMaterialBindGroupIndex);

    uniformData.setLayout(kaLayout);

    for (auto& [k, v] : matRes.getUniformValues()) {
        std::visit([&](const auto& val) { uniformData.setData(k, val); }, v);
    }

    {
        std::vector<wgpu::BindGroupEntry> entries;

        for (auto& res : kaColorProgram->getReflection()) {
            if (res.bindGroup != kMaterialBindGroupIndex) {
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

            case ResourceType::kSampler:
            case ResourceType::kComparisonSampler: {
                auto* search = findByID(matRes.getSamplerDescriptors(), id);

                LEMON_ASSERT(search != nullptr,
                             "failed to create bind group: binding not found for sampler. id: ", id);

                auto kaSampler = pMaterialMan->getSampler(*search);
                entries.emplace_back(BindingInitializationHelper(binding, *kaSampler).getAsBinding());

                // Store the keep-alive for the lifetime of this instance.
                kaSamplers.emplace_back(kaSampler);

                break;
            }

            case ResourceType::kSampledTexture:
            case ResourceType::kMultisampledTexture:
            case ResourceType::kDepthTexture:
            case ResourceType::kDepthMultisampledTexture: {
                auto* search = findByID(matRes.getTextureLocations(), id);

                LEMON_ASSERT(search != nullptr,
                             "failed to create bind group: binding not found for texture. id: ", id);

                auto* pTextureRes = pResourceMan->getResource<TextureResource>(search->handle);

                LEMON_ASSERT(pTextureRes != nullptr,
                             "failed to create bind group: texture resource not available. id: ", id);

                auto kaTexture = pMaterialMan->getTexture(*pTextureRes);
                auto view = kaTexture->CreateView();

                entries.emplace_back(BindingInitializationHelper(binding, view).getAsBinding());

                // Store the keep-alive for the lifetime of this instance.
                kaTextures.emplace_back(kaTexture);

                break;
            }

            default:
                // kStorageBuffer,
                // kReadOnlyStorageBuffer,
                // kReadOnlyStorageTexture,
                // kWriteOnlyStorageTexture,
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
}

size_t
folly::hasher<lemon::render::MaterialResourceDescriptor>::operator()(
    const lemon::render::MaterialResourceDescriptor& data) const
{
    lemon::Hash hash;
    hash.append(data.pResource->getHandle(), data.meshComponents);
    return hash;
}
