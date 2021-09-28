#include "shader.h"

#include <lemon/shared.h>
#include <lemon/utils/utils.h>
#include <lemon/shared/filesystem.h>

#include <inja/inja.hpp>
#include <inja/environment.hpp>

#include <dawn/webgpu_cpp.h>
#include <dawn/webgpu_cpp_print.h>
#include <dawn/dawn_proc.h>
#include <dawn_native/D3D12Backend.h>

#include <lemon-shader/reflection.h>

//#include <tint/tint.h>

//#undef INCLUDE_TINT_TINT_H_
//#include <src/ast/module.h>
//#include <src/sem/function.h>
//#define INCLUDE_TINT_TINT_H_

//#include "src/ast/bool_literal.h"
//#include "src/ast/call_expression.h"
//#include "src/ast/float_literal.h"
//#include "src/ast/interpolate_decoration.h"
//#include "src/ast/location_decoration.h"
//#include "src/ast/module.h"
//#include "src/ast/override_decoration.h"
//#include "src/ast/scalar_constructor_expression.h"
//#include "src/ast/sint_literal.h"
//#include "src/ast/uint_literal.h"
//#include "src/sem/array.h"
//#include "src/sem/call.h"
//#include "src/sem/depth_multisampled_texture_type.h"
//#include "src/sem/f32_type.h"
//#include "src/sem/function.h"
//#include "src/sem/i32_type.h"
//#include "src/sem/matrix_type.h"
//#include "src/sem/multisampled_texture_type.h"
//#include "src/sem/sampled_texture_type.h"
//#include "src/sem/statement.h"
//#include "src/sem/storage_texture_type.h"
//#include "src/sem/struct.h"
//#include "src/sem/u32_type.h"
//#include "src/sem/variable.h"
//#include "src/sem/vector_type.h"
//#include "src/sem/void_type.h"
//#include "src/utils/math.h"
//#include "src/utils/unique_vector.h"

using TemplateParams = inja::json;

enum class ShaderCompilationError { Unknown };

class ShaderByteCode {
};

class ShaderSourceCode {
    std::string code;

public:
    explicit ShaderSourceCode(std::string&& inCode) : code{inCode} {}

    uint64_t
    getHash()
    {
        return 0;
    }

    tl::expected<ShaderByteCode, ShaderCompilationError>
    compile()
    {
        return ShaderByteCode();
    }

    inline std::string&
    string()
    {
        return code;
    }
};

class ShaderTemplateParser {
    std::filesystem::path baseDir;
    inja::Environment tplEnv;

public:
    ShaderTemplateParser(std::filesystem::path& inBaseDir) : tplEnv{}
    {
        baseDir = inBaseDir;

        tplEnv.set_line_statement("//#");
        tplEnv.set_search_included_templates_in_files(false);
        tplEnv.set_include_callback([this](auto&, auto& name) {
            auto fullPath = baseDir / name;
            return loadShaderCode(fullPath);
        });

        tplEnv.add_callback("bind", 4, [](inja::Arguments& args) {
            // group
            // binding
            // name
            // type

            /*

            struct MyData {
                mat1: mat4x4<f32>,
                mat2: mat4x4<f32>
            };

            {{ binding(2, 0, "albedo") }}
            var surfaceTextureAlbedo: texture_2d<f32>;

            {{ binding(3, 0, "matrices") }}
            var matrices: MyData;

            */

            int number = args.at(0)->get<int>(); // Adapt the index and type of the argument
            return 2 * number;
        });
    }

    inline ShaderSourceCode
    getShaderCode(std::filesystem::path& fullPath, TemplateParams& params)
    {
        auto tpl = loadShaderCode(fullPath);
        return ShaderSourceCode(std::move(tplEnv.render(tpl, params)));
    }

private:
    inja::Template
    loadShaderCode(std::filesystem::path& path)
    {
        auto res = lemon::io::readTextFile(path);
        if (res) {
            return tplEnv.parse(*res);
        } else {
            std::string strErr = "Failed to resolve shader include: [" + path.string() +
                                 "] error: " + std::to_string((int)res.error());
            lemon::utils::logErr(strErr);
            return tplEnv.parse(strErr);
        }
    }
};

void
testShader()
{
    std::filesystem::path templateBaseDir = R"(C:\git\lemon\resources\internal\shaders)";
    ShaderTemplateParser parser(templateBaseDir);

    TemplateParams params;
    params["TEXCOORD1"] = false;

    auto codePath = templateBaseDir / "BaseSurfacePBR.wgsl";
    auto code = parser.getShaderCode(codePath, params);

    lemon::utils::log(code.string());
    // lemon::utils::log("shader code rendered");

    auto pathStr = codePath.string();
    lemon::shader::ShaderProgram shaderProgram(pathStr, code.string());

    if (shaderProgram.isValid()) {
        auto bindingReflection = shaderProgram.getBindingReflection();
        lemon::utils::log("found bindings: ", bindingReflection.size());
    } else {
        lemon::utils::logErr("program invalid: ", shaderProgram.getDiagnostic());
    }

    /*

    // templated, internally sanity checks data size
    material->setData(
        "myMatParams.scrollSpeed",
        glm::vec2(5.f, 5.f)
    );

    maintain map of [name hash] => {
        group index,
        location index,
        internal offset
    }

    */
}