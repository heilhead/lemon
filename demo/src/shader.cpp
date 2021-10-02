#include "shader.h"

#include <lemon/shared.h>
#include <lemon/utils/utils.h>
#include <lemon/shared/filesystem.h>
#include <lemon/engine.h>
#include <lemon/resource/ResourceManager.h>
#include <lemon/resource/types/MaterialResource.h>

#include <inja/inja.hpp>
#include <inja/environment.hpp>

#include <dawn/webgpu_cpp.h>
#include <dawn/webgpu_cpp_print.h>
#include <dawn/dawn_proc.h>
#include <dawn_native/D3D12Backend.h>

#include <lemon-shader/ShaderReflection.h>

using namespace lemon::res;

void
testShader()
{
    lemon::Engine engine;
    std::string assetPath(R"(C:\git\lemon\resources)");
    engine.init(assetPath);

    auto* pSchedMan = Scheduler::get();
    auto* pResMan = ResourceManager::get();

    // Scheduler schedMan(1, 1);
    // std::filesystem::path baseDir = R"(C:\git\lemon\resources\)";
    // ResourceManager resMan(std::move(baseDir));

    ResourceLocation matLoc(R"(a\M_A)");
    auto result = pSchedMan->block(pResMan->loadResource<MaterialResource>(matLoc));
    if (result) {
        material::BlueprintConfiguration config;
        config.define("TEXCOORD1", false);
        auto shader = (*result)->getShader(config);
        auto refl = shader->getReflection();
        lemon::utils::log("material loaded");
    } else {
        lemon::utils::logErr("material failed to load: ", (int)result.error());
    }

    // auto& composer = resMan.getShaderComposer();

    // lemon::res::material::ShaderConfiguration config;
    // config.define("TEXCOORD1", false);

    // auto codePath = baseDir / "internal" / "shaders" / "BaseSurfacePBR.wgsl";
    // auto blueprint = composer.getBlueprint(codePath);
    // if (blueprint) {
    //     auto code = (*blueprint).getCode(config);
    //     auto pathStr = codePath.string();

    //    lemon::shader::ShaderProgram program(pathStr, code);

    //    if (program.isValid()) {
    //        auto bindingReflection = program.getBindingReflection();
    //        lemon::utils::log("found bindings: ", bindingReflection.size());
    //    } else {
    //        lemon::utils::logErr(code);
    //        lemon::utils::logErr("program invalid: ", program.getDiagnostic());
    //    }
    //} else {
    //    lemon::utils::logErr("failed to compose shader blueprint: ", (int)blueprint.error());
    //}
}