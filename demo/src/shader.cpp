#include "shader.h"

#include <cassert>
#include <lemon/shared.h>
#include <lemon/utils/utils.h>
#include <lemon/shared/filesystem.h>
#include <lemon/engine.h>
#include <lemon/render/RenderManager.h>
#include <lemon/resource/ResourceManager.h>
#include <lemon/resource/types/MaterialResource.h>
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

void
testShader()
{
    lemon::Engine engine;
    std::string assetPath(R"(C:\git\lemon\resources)");
    engine.init(assetPath);

    auto* pScheduler = Scheduler::get();
    auto* pResMan = ResourceManager::get();

    // Scheduler schedMan(1, 1);
    // std::filesystem::path baseDir = R"(C:\git\lemon\resources\)";
    // ResourceManager resMan(std::move(baseDir));

    ResourceLocation matLoc("a\\M_A");
    auto result = pScheduler->block(pResMan->loadResource<MaterialResource>(matLoc));
    if (result) {
        material::MaterialConfiguration config;
        config.define("TEXCOORD1", false);
        auto& material = **result;
        
        auto* sampler = material.getSamplerDescriptor(lemon::sid("mySampler1"));
        LEMON_ASSERT(sampler != nullptr);

        auto* texture = material.getTextureLocation(lemon::sid("normal"));
        LEMON_ASSERT(texture != nullptr);

        auto* uniform = material.getUniformValue(lemon::sid("lemonData.lemonVecData"));
        LEMON_ASSERT(uniform != nullptr);

        auto shader = MaterialManager::get()->getShader(material, config);
        auto& refl = shader->getReflection();
        auto bgl = MaterialManager::get()->getBindGroupLayout(*shader);
        lemon::utils::log("material loaded");
    } else {
        lemon::utils::logErr("material failed to load: ", (int)result.error());
    }
}