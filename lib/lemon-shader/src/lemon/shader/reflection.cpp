#include <lemon/shader/reflection.h>

using namespace lemon::shader;

std::vector<ResourceBindingDescriptor>
lemon::shader::createReflection(const std::string& path, const std::string& sourceCode)
{
    return ShaderReflection(path, sourceCode).getBindingReflection();
}
