#include <lemon/resource/ResourceMetadata.h>
#include <lemon/resource/types/MaterialResource.h>
#include <lemon/resource/ResourceManager.h>
#include <lemon/resource/types/material/MaterialBlueprint.h>
#include <lemon/render/material/ShaderProgram.h>
#include <lemon/render/RenderManager.h>
#include <lemon/device/Device.h>
#include <lemon/utils/utils.h>
#include <lemon/shared/assert.h>

using namespace lemon::utils;
using namespace lemon::device;
using namespace lemon::res;
using namespace lemon::res::material;
using namespace lemon::render;

uint64_t
computeHash(const std::optional<MaterialBlueprint>& blueprint, const MaterialConfiguration& config)
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
        samplers.insert({lemon::sid(k), v});
    }

    for (const auto& [k, v] : pMeta->textures) {
        textures.insert({lemon::sid(k), ResourceLocation(v)});
    }

    for (const auto& [k, v] : pMeta->uniforms) {
        uniforms.insert({lemon::sid(k), v});
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

const MaterialResource::SamplerDescriptor*
MaterialResource::getSamplerDescriptor(StringID id) const
{
    auto search = samplers.find(id);
    if (search == samplers.end()) {
        return nullptr;
    }

    return &search->second;
}

const ResourceLocation*
MaterialResource::getTextureLocation(StringID id) const
{
    auto search = textures.find(id);
    if (search == textures.end()) {
        return nullptr;
    }

    return &search->second;
}

const MaterialResource::UniformValue*
MaterialResource::getUniformValue(StringID id) const
{
    auto search = uniforms.find(id);
    if (search == uniforms.end()) {
        return nullptr;
    }

    return &search->second;
}
