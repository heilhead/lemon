#include "shader.h"

#include <lemon/shared.h>
#include <lemon/utils/utils.h>
#include <lemon/shared/filesystem.h>
#include <lemon/resource/ResourceManager.h>

#include <inja/inja.hpp>
#include <inja/environment.hpp>

#include <dawn/webgpu_cpp.h>
#include <dawn/webgpu_cpp_print.h>
#include <dawn/dawn_proc.h>
#include <dawn_native/D3D12Backend.h>

#include <lemon-shader/reflection.h>

using namespace lemon::res;

void
testShader()
{
    std::filesystem::path baseDir = R"(C:\git\lemon\resources\)";
    ResourceManager resMan(std::move(baseDir));
    auto& composer = resMan.getShaderComposer();

    lemon::res::material::ComposerDefinitions defs;
    defs["TEXCOORD1"] = false;

    auto codePath = baseDir / "internal" / "shaders" / "BaseSurfacePBR.wgsl";
    auto blueprint = composer.getBlueprint(codePath);
    if (blueprint) {
        auto code = (*blueprint).getCode(defs);
        auto pathStr = codePath.string();

        lemon::shader::ShaderProgram program(pathStr, code);

        if (program.isValid()) {
            auto bindingReflection = program.getBindingReflection();
            lemon::utils::log("found bindings: ", bindingReflection.size());
        } else {
            lemon::utils::logErr(code);
            lemon::utils::logErr("program invalid: ", program.getDiagnostic());
        }
    } else {
        lemon::utils::logErr("failed to compose shader blueprint: ", (int)blueprint.error());
    }
}