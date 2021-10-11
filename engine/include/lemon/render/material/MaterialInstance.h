#pragma once

#include <lemon/render/material/MaterialUniformData.h>
#include <lemon/render/MeshVertexFormat.h>
#include <lemon/shared/NonCopyable.h>
#include <folly/small_vector.h>

namespace lemon::res {
    class MaterialResource;
}

namespace lemon::render {
    class MeshVertexFormat;
    class ShaderProgram;

    struct MaterialResourceDescriptor {
        const res::MaterialResource* pResource;
        MeshComponents meshComponents;
    };

    class MaterialSharedResources : NonMovable {
        friend class MaterialInstance;

        // wgpu::RenderPipeline mainPipeline;
        // wgpu::RenderPipeline depthPipeline;
        wgpu::BindGroup bindGroup;

        // Default uniform data.
        MaterialUniformData uniformData;

        // Keep-alive references to hold the required resources.
        folly::small_vector<KeepAlive<wgpu::Sampler>, 4> kaSamplers;
        folly::small_vector<KeepAlive<wgpu::Texture>, 8> kaTextures;
        KeepAlive<MaterialLayout> kaLayout;
        KeepAlive<ShaderProgram> kaMainProgram;
        KeepAlive<ShaderProgram> kaDepthProgram;

    public:
        MaterialSharedResources(const res::MaterialResource& matRes, const MeshVertexFormat& vertexFormat);
    };

    class MaterialInstance {
        MaterialUniformData uniformData;
        KeepAlive<MaterialSharedResources> kaSharedResources;

    public:
        MaterialInstance() = default;
        MaterialInstance(const MaterialInstance& other) = default;
        MaterialInstance(MaterialInstance&& other) = default;

        MaterialInstance&
        operator=(const MaterialInstance& other) = default;
        MaterialInstance&
        operator=(MaterialInstance&& other) = default;

        MaterialInstance(KeepAlive<MaterialSharedResources> kaSharedResources)
            : kaSharedResources{kaSharedResources}
        {
            uniformData = kaSharedResources->uniformData;
        }

        inline bool
        isValid() const
        {
            return kaSharedResources;
        }
    };
} // namespace lemon::render

template<>
struct folly::hasher<lemon::render::MaterialResourceDescriptor> {
    using folly_is_avalanching = std::true_type;

    size_t
    operator()(const lemon::render::MaterialResourceDescriptor& data) const;
};