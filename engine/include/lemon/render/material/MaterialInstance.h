#pragma once

#include <concepts>
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
    class MeshSurfacePipeline;

    struct MaterialResourceDescriptor {
        const res::MaterialResource* pResource;
        MeshComponents meshComponents;
    };

    class MaterialSharedResources : NonMovable {
        friend class MaterialInstance;
        friend class PipelineManager;
        friend class MeshSurfacePipeline;

        // Bind group is the only resource directly owned by `MaterialSharedResources`.
        // The rest are stored elsewhere and reused between materials.
        wgpu::BindGroup bindGroup;

        // Uniform data containing the defaults. Copied to each `MaterialInstance` at creation.
        MaterialUniformData uniformData;

        // Keep-alive shared references to hold the required resources.
        folly::small_vector<KeepAlive<wgpu::Sampler>, 4> kaSamplers;
        folly::small_vector<KeepAlive<wgpu::Texture>, 8> kaTextures;
        KeepAlive<MaterialLayout> kaLayout;
        KeepAlive<ShaderProgram> kaColorProgram;
        KeepAlive<ShaderProgram> kaDepthProgram;
        KeepAlive<MeshSurfacePipeline> kaPipeline;

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

        inline const MeshSurfacePipeline&
        getRenderPipeline() const
        {
            LEMON_ASSERT(isValid());
            return *kaSharedResources->kaPipeline;
        }

        // TODO: Figure out a better interface.
        template<std::integral TData>
        inline void
        setParameter(StringID id, const TData& val)
        {
            LEMON_ASSERT(isValid());
            uniformData.setData(id, val);
        }

        inline const MaterialUniformData&
        getUniformData() const
        {
            LEMON_ASSERT(isValid());
            return uniformData;
        }

        inline MaterialUniformData&
        getUniformData()
        {
            LEMON_ASSERT(isValid());
            return uniformData;
        }

        inline const wgpu::BindGroup&
        getBindGroup() const
        {
            LEMON_ASSERT(isValid());
            return kaSharedResources->bindGroup;
        }
    };
} // namespace lemon::render

template<>
struct folly::hasher<lemon::render::MaterialResourceDescriptor> {
    using folly_is_avalanching = std::true_type;

    size_t
    operator()(const lemon::render::MaterialResourceDescriptor& data) const;
};
