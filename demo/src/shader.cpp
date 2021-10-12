#include "shader.h"

#include <cassert>
#include <lemon/shared.h>
#include <lemon/shared/utils.h>
#include <lemon/shared/filesystem.h>
#include <lemon/engine.h>
#include <lemon/render/RenderManager.h>
#include <lemon/render/material/MaterialUniformData.h>
#include <lemon/resource/ResourceManager.h>
#include <lemon/resource/types/MaterialResource.h>
#include <lemon/resource/types/TextureResource.h>
#include <lemon/resource/types/ModelResource.h>
#include <lemon/shared/Hash.h>

#include <inja/inja.hpp>
#include <inja/environment.hpp>

#include <dawn/webgpu_cpp.h>
#include <dawn/webgpu_cpp_print.h>
#include <dawn/dawn_proc.h>
#include <dawn_native/D3D12Backend.h>

#include <lemon/shader/reflection.h>

using namespace lemon::res;
using namespace lemon::render;
using namespace lemon;

namespace shader_test {
    const ModelResource::Model*
    loadModel()
    {
        ResourceLocation location(R"(ozz-sample\MannequinSkeleton.lem:SK_Mannequin)");

        auto result = Scheduler::get()->block(
            ResourceManager::get()->loadResource<ModelResource>(location, ResourceLifetime::Static));

        LEMON_ASSERT(result.has_value());

        return (*result)->getObject<ModelResource::Model>(location.object);
    }

    const MaterialResource*
    loadMaterial()
    {
        ResourceLocation location(R"(misc\\M_Basketball)");

        auto result = Scheduler::get()->block(
            ResourceManager::get()->loadResource<MaterialResource>(location, ResourceLifetime::Static));

        LEMON_ASSERT(result.has_value());

        return *result;
    }
} // namespace shader_test

void
testShader()
{
    lemon::Engine engine;
    std::string assetPath(R"(C:\git\lemon\resources)");
    engine.init(assetPath);

    auto* pScheduler = Scheduler::get();
    auto* pResMan = ResourceManager::get();
    auto* pMatMan = MaterialManager::get();

    auto* pMaterial = shader_test::loadMaterial();
    auto* pModel = shader_test::loadModel();

    LEMON_ASSERT(pMaterial != nullptr);
    LEMON_ASSERT(pModel != nullptr);

    auto& material = *pMaterial;
    auto& model = *pModel;

    auto matInstance = pMatMan->getMaterialInstance(material, model.getMeshes()[0]->vertexFormat);

    LEMON_ASSERT(matInstance.isValid());

    auto& pipeline = matInstance.getRenderPipeline();

    // constexpr auto lemonVecData = lemon::sid("lemonData.lemonVecData");

    logger::log("finished!");
}
