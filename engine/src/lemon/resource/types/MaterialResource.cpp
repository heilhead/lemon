#include <lemon/resource/ResourceMetadata.h>
#include <lemon/resource/types/MaterialResource.h>
#include <lemon/resource/ResourceManager.h>
#include <lemon/resource/types/material/MaterialBlueprint.h>
#include <lemon/render/material/ShaderProgram.h>
#include <lemon/render/RenderManager.h>
#include <lemon/device/Device.h>
#include <lemon/shared/utils.h>
#include <lemon/shared/logger.h>

using namespace lemon;
using namespace lemon::utils;
using namespace lemon::device;
using namespace lemon::res;
using namespace lemon::res::material;
using namespace lemon::render;

uint64_t
computeHash(const std::optional<MaterialBlueprint>& blueprint, const render::MaterialConfiguration& config)
{
    lemon::Hash hash;

    hash.append(config);

    if (blueprint) {
        hash.appendHash(blueprint->getHash());
    }

    return hash;
}

Task<MaterialBlueprint, ResourceLoadingError>
loadShaderBlueprint(std::string& bplPath)
{
    auto* pResMan = ResourceManager::get();
    auto fullPath = pResMan->resolvePath(bplPath);
    auto& composer = pResMan->getMaterialComposer();

    co_return composer.getBlueprint(fullPath).map_error(
        [](auto err) { return ResourceLoadingError::DataMissing; });
}

MaterialResource::MaterialResource()
{
    logger::log("MaterialResource::MaterialResource()");
}

MaterialResource::~MaterialResource()
{
    logger::log("MaterialResource::~MaterialResource()");
}

VoidTask<ResourceLoadingError>
MaterialResource::load(ResourceMetadata&& md)
{
    auto* pMeta = md.get<Metadata>();
    domain = pMeta->domain;

    for (const auto& [k, v] : pMeta->definitions) {
        config.define(k, v);
    }

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
        LEMON_TODO();
    }

    co_return {};
}
