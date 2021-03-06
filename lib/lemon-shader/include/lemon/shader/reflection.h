#pragma once

#include <string>
#include <lemon/shader/ShaderReflection.h>

namespace lemon::shader {
    std::vector<ResourceBindingDescriptor>
    createReflection(const std::string& path, const std::string& sourceCode);
} // namespace lemon::shader
