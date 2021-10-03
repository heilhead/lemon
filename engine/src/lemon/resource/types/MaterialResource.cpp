#include <lemon/resource/ResourceMetadata.h>
#include <lemon/resource/types/MaterialResource.h>
#include <lemon/resource/ResourceManager.h>
#include <lemon/resource/types/material/MaterialBlueprint.h>
#include <lemon/render/ShaderProgram.h>
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
    lemon::utils::log("MaterialResource::MaterialResource()");
}

MaterialResource::~MaterialResource()
{
    lemon::utils::log("MaterialResource::~MaterialResource()");
}

VoidTask<ResourceLoadingError>
MaterialResource::load(ResourceMetadata&& inMetadata)
{
    metadata = std::move(*inMetadata.get<Metadata>());
    if (metadata.baseType == BaseType::Shader) {
        auto result = co_await IOTask(loadShaderBlueprint(metadata.basePath));
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

uint64_t
MaterialResource::computeShaderHash(const MaterialConfiguration& pipelineConfig) const
{
    MaterialConfiguration finalConfig = config;
    finalConfig.merge(pipelineConfig);
    return computeHash(blueprint, finalConfig);
}

const ShaderProgram*
MaterialResource::getShader(const MaterialConfiguration& pipelineConfig)
{
    MaterialConfiguration finalConfig = config;
    finalConfig.merge(pipelineConfig);
    uint64_t hash = computeHash(blueprint, finalConfig);

    auto [pProgram, bInserted] = ResourceManager::get()->getShaderCache().findOrInsert(hash, [&]() {
        ShaderProgram* pProgram;

        if (blueprint) {
            // TODO: Handle compilation errors, e.g. `template variable not found`
            auto sourceCode = blueprint->renderShaderSource(finalConfig);
            pProgram = RenderManager::get()->getShaderCompiler().compile(hash, sourceCode).release();
        } else {
            // Create an unitialized and invalid program.
            pProgram = new ShaderProgram{hash};
        }

        return pProgram;
    });

    return pProgram;
}
