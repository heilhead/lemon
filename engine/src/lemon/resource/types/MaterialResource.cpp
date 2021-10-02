#include <lemon/resource/ResourceMetadata.h>
#include <lemon/resource/types/MaterialResource.h>
#include <lemon/resource/ResourceManager.h>
#include <lemon/resource/types/material/Blueprint.h>
#include <lemon/render/ShaderProgram.h>
#include <lemon/device/Device.h>
#include <lemon/utils/utils.h>

using namespace lemon::utils;
using namespace lemon::device;
using namespace lemon::res;
using namespace lemon::res::material;
using namespace lemon::render;

uint64_t
computeShaderHash(const std::optional<Blueprint>& blueprint, const BlueprintConfiguration& config)
{
    lemon::Hash h;

    h.append(config);

    if (blueprint) {
        h.append(*blueprint);
    }

    return h;
}

Task<Blueprint, ResourceLoadingError>
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
    if (metadata.baseType == MaterialBaseType::Shader) {
        auto result = co_await IOTask(loadShaderBlueprint(metadata.basePath));
        if (result) {
            blueprint = std::move(*result);
        } else {
            co_return result.error();
        }
    } else {
        assert(false && "not implemented");
    }

    co_return {};
}

const ShaderProgram*
MaterialResource::getShader(const BlueprintConfiguration& pipelineConfig)
{
    BlueprintConfiguration finalConfig = config;
    finalConfig.merge(pipelineConfig);

    uint64_t programHash = computeShaderHash(blueprint, finalConfig);

    auto& cache = ResourceManager::get()->getShaderCache();
    auto [pProgram, bInserted] = cache.findOrInsert(programHash, [&]() {
        if (blueprint) {
            // TODO: Handle compilation errors, e.g. `template variable not found`
            auto sourceCode = blueprint->renderShaderSource(finalConfig);
            return Device::get()->getGPU()->compileShaderProgram(sourceCode).release();
        } else {
            // Create an unitialized and invalid program.
            return new ShaderProgram{};
        }
    });

    return pProgram;
}
