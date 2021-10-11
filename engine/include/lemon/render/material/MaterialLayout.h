#pragma once

#include <cstdint>
#include <lemon/shared/Memory.h>
#include <lemon/shared/AtomicCache.h>
#include <lemon/render/material/common.h>
#include <lemon/render/material/ShaderProgram.h>

namespace lemon::res {
    class MaterialResource;
}

namespace lemon::render {
    struct MaterialUniformLayout {
        static constexpr size_t kMaxItems = kMaterialMaxUniforms * kMaterialMaxUniformMembers;

        struct Member {
            uint64_t id;
            uint32_t offset;
            uint32_t size;
        };

        struct Uniform {
            Member members[kMaterialMaxUniformMembers];
            StringID id;
            uint32_t size;
            uint8_t memberCount = 0;
        };

        Uniform uniforms[kMaterialMaxUniforms];
        uint32_t totalSize = 0;
        uint8_t uniformCount = 0;

        MaterialUniformLayout(const ShaderProgram& program, uint32_t bindGroup);
    };

    struct MaterialLayout {
        wgpu::BindGroupLayout bindGroupLayout;
        MaterialUniformLayout uniformLayout;

        MaterialLayout(const lemon::res::MaterialResource& material, const ShaderProgram& program,
                       uint8_t bindGroupIndex);
    };
} // namespace lemon::render
