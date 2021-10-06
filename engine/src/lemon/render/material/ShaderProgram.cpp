#include <lemon/render/material/ShaderProgram.h>
#include <lemon/render/RenderManager.h>
#include <lemon-shader/reflection.h>

using namespace lemon::render;
using namespace lemon::shader;

uint64_t
ShaderProgram::createReflection(const std::string& path, const std::string& sourceCode)
{
    reflection = std::move(lemon::shader::createReflection(path, sourceCode));
    reflectionHash = lemon::hash(reflection);
    return reflectionHash;
}