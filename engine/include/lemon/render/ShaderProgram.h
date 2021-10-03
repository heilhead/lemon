#pragma once

#include <cassert>
#include <vector>
#include <lemon-shader/ShaderReflection.h>
#include <lemon/shared/Hash.h>
#include <dawn/webgpu_cpp.h>
#include <lemon/shared/assert.h>

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
        uint64_t moduleHash;
        uint64_t reflectionHash;

    public:
        ShaderProgram() : moduleHash{0} {}

        ShaderProgram(uint64_t moduleHash) : moduleHash{moduleHash} {}

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

        uint64_t
        createReflection(const std::string& path, const std::string& sourceCode);
    };
} // namespace lemon::render

template<>
struct folly::hasher<lemon::shader::StructMember> {
    using folly_is_avalanching = std::true_type;

    size_t
    operator()(const lemon::shader::StructMember& data) const
    {
        lemon::Hash hash;
        hash.append(data.name, data.offset, data.size, data.align);
        return hash;
    }
};

template<>
struct folly::hasher<lemon::shader::ResourceBindingDescriptor> {
    using folly_is_avalanching = std::true_type;

    size_t
    operator()(const lemon::shader::ResourceBindingDescriptor& data) const
    {
        lemon::Hash hash;

        hash.append(data.name, data.stage, data.resourceType, data.bindGroup, data.binding);

        switch (data.resourceType) {
        case lemon::shader::ResourceType::kUniformBuffer:
            hash.append(data.size, data.sizeNoPadding, data.align);
            hash.append(data.members);
            break;
        case lemon::shader::ResourceType::kDepthTexture:
        case lemon::shader::ResourceType::kDepthMultisampledTexture:
        case lemon::shader::ResourceType::kSampledTexture:
        case lemon::shader::ResourceType::kMultisampledTexture:
        case lemon::shader::ResourceType::kExternalTexture:
            // Note: `imageFormat` currently contains garbage and will break hashing.
            // Use it once reflection data is fixed.
            hash.append(data.dim, data.sampledKind);
            break;
        case lemon::shader::ResourceType::kSampler:
        case lemon::shader::ResourceType::kComparisonSampler:
            // Nothing to configure for samplers.
            break;
        case lemon::shader::ResourceType::kStorageBuffer:
        case lemon::shader::ResourceType::kReadOnlyStorageBuffer:
        case lemon::shader::ResourceType::kReadOnlyStorageTexture:
        case lemon::shader::ResourceType::kWriteOnlyStorageTexture:
            LEMON_TODO();
            break;
        };

        return hash;
    }
};
