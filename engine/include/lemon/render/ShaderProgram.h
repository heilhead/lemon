#pragma once

#include <cassert>
#include <vector>
#include <lemon-shader/ShaderReflection.h>
#include <dawn/webgpu_cpp.h>

namespace lemon::render {
    class ShaderProgram {
        friend class ShaderCompiler;

        using ResourceDescriptor = shader::ResourceBindingDescriptor;

    public:
        struct DiagnosticMessage {
            std::string message;
            WGPUCompilationMessageType type;
            uint64_t lineNum;
            uint64_t linePos;
            uint64_t offset;
            uint64_t length;
        };

    private:
        wgpu::ShaderModule shaderModule;
        bool valid = false;
        std::vector<DiagnosticMessage> diagnostics;
        std::vector<ResourceDescriptor> reflection;

    public:
        inline bool
        isValid() const
        {
            return valid;
        }

        inline const wgpu::ShaderModule&
        getModule() const
        {
            return shaderModule;
        }

        inline const std::vector<DiagnosticMessage>&
        getDiagnostics() const
        {
            return diagnostics;
        }

        inline const std::vector<ResourceDescriptor>&
        getReflection() const
        {
            return reflection;
        }
    };
} // namespace lemon::render