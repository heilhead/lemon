#include <lemon/resource/ResourceMetadata.h>
#include <lemon/resource/ResourceManager.h>
#include <lemon/resource/types/MaterialResource.h>
#include <lemon/resource/types/material/MaterialBlueprint.h>
#include <lemon/render/material/ShaderProgram.h>
#include <lemon/render/RenderManager.h>
#include <lemon/device/Device.h>

using namespace lemon;
using namespace lemon::utils;
using namespace lemon::device;
using namespace lemon::res;
using namespace lemon::res::material;
using namespace lemon::render;

template<typename TValue>
inline bool
findKey(const MaterialResource::ResourceList<TValue>& vec, StringID key)
{
    for (auto& [k, v] : vec) {
        if (k == key) {
            return true;
        }
    }

    return false;
}

template<typename TValue>
void
mergeResources(const MaterialResource::ResourceList<TValue>& src, MaterialResource::ResourceList<TValue>& dst)
{
    for (auto& [srcKey, srcVal] : src) {
        if (!findKey(dst, srcKey)) {
            dst.emplace_back(std::make_pair(srcKey, srcVal));
        }
    }
}

MaterialResource::MaterialResource() {}

MaterialResource::~MaterialResource() {}

VoidTask<ResourceLoadingError>
MaterialResource::load(ResourceMetadata&& md)
{
    auto* pMeta = md.get<Metadata>();
    domain = pMeta->domain;

    for (const auto& [k, v] : pMeta->samplers) {
        samplers.emplace_back(std::make_pair(lemon::sid(k), v));
    }

    for (const auto& [k, v] : pMeta->textures) {
        textures.emplace_back(std::make_pair(lemon::sid(k), ResourceLocation(v)));
    }

    for (const auto& [k, v] : pMeta->uniforms) {
        uniforms.emplace_back(std::make_pair(lemon::sid(k), v));
    }

    if (pMeta->baseType == BaseType::Shader) {
        auto result = co_await IOTask(loadShaderBlueprint(pMeta->basePath));
        if (result) {
            blueprint = std::move(*result);
        } else {
            co_return result.error();
        }
    } else {
        const auto* pResourceMan = ResourceManager::get();
        const auto* pParentMat = pResourceMan->getResource<MaterialResource>(ResourceHandle(pMeta->basePath));

        LEMON_ASSERT(pParentMat != nullptr);

        blueprint = pParentMat->blueprint;
        config = pParentMat->config;

        mergeResources(pParentMat->samplers, samplers);
        mergeResources(pParentMat->textures, textures);
        mergeResources(pParentMat->uniforms, uniforms);
    }

    config.define(kShaderDefineMaterialLighting, domain.shadingModel == ShadingModel::Lit);

    for (const auto& [k, v] : pMeta->definitions) {
        config.define(k, v);
    }

    co_return {};
}

Task<MaterialBlueprint, ResourceLoadingError>
MaterialResource::loadShaderBlueprint(const std::string& bplPath)
{
    auto* pResMan = ResourceManager::get();
    auto fullPath = pResMan->resolvePath(bplPath);
    auto& composer = pResMan->getMaterialComposer();

    co_return composer.getBlueprint(fullPath).map_error(
        [](auto) { return ResourceLoadingError::DataMissing; });
}
