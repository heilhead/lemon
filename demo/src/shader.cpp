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
#include <lemon/shared/Hash.h>

#include <inja/inja.hpp>
#include <inja/environment.hpp>

#include <dawn/webgpu_cpp.h>
#include <dawn/webgpu_cpp_print.h>
#include <dawn/dawn_proc.h>
#include <dawn_native/D3D12Backend.h>

#include <lemon-shader/ShaderReflection.h>

using namespace lemon::res;
using namespace lemon::render;
using namespace lemon;

void
testShader()
{
    lemon::Engine engine;
    std::string assetPath(R"(C:\git\lemon\resources)");
    engine.init(assetPath);

    auto* pScheduler = Scheduler::get();
    auto* pResMan = ResourceManager::get();
    auto* pMatMan = MaterialManager::get();

    // ResourceLocation matLoc("misc\\T_Basketball_D.png");
    // auto result = pScheduler->block(pResMan->loadResource<TextureResource>(matLoc));

    ResourceLocation matLoc("misc\\M_Basketball");
    auto result = pScheduler->block(pResMan->loadResource<MaterialResource>(matLoc));
    if (result) {
        material::MaterialConfiguration config;
        config.define("TEXCOORD1", false);
        auto& material = **result;

        // auto* sampler = material.getSamplerDescriptor(lemon::sid("surfaceSampler"));
        // LEMON_ASSERT(sampler != nullptr);

        // auto* textureLoc = material.getTextureLocation(lemon::sid("surfaceTexture"));
        // LEMON_ASSERT(textureLoc != nullptr);

        // auto texResult = pScheduler->block(pResMan->loadResource<TextureResource>(*textureLoc));
        // LEMON_ASSERT(texResult);

        // auto& texture = **texResult;

        // auto* uniform = material.getUniformValue(lemon::sid("lemonData.lemonVecData"));
        // LEMON_ASSERT(uniform != nullptr);

        auto shader = pMatMan->getShader(material, config);
        // auto gpuTex = pMatMan->getTexture(texture);
        // auto& refl = shader->getReflection();
        auto kaMatLayout = pMatMan->getMaterialLayout(material, *shader);
        auto& matLayout = *kaMatLayout;

        MaterialUniformData uniData(kaMatLayout);
        uniData.setData(lemon::sid("lemonData.lemonVecData"), glm::vec4(1.f, 1.f, 1.f, 1.f));

        logger::log("material loaded");
    } else {
        logger::err("material failed to load: ", (int)result.error());
    }
}
