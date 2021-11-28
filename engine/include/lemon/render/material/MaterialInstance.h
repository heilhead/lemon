#pragma once

#include <lemon/render/material/MaterialUniformData.h>
#include <lemon/render/MeshVertexFormat.h>

namespace lemon::res {
    class MaterialResource;
}

namespace lemon::render {
    class MeshVertexFormat;
    class ShaderProgram;
    class SurfacePipeline;
    class PostProcessPipeline;
    class DynamicPipeline;

    struct MaterialResourceDescriptor {
        const res::MaterialResource* pResource;
        MeshComponents meshComponents;
    };

    struct DynamicMaterialResourceDescriptor {
        folly::small_vector<std::pair<StringID, wgpu::Sampler>, 4> samplers;
        folly::small_vector<std::pair<StringID, wgpu::TextureView>, 8> textures;
        folly::small_vector<std::pair<StringID, wgpu::Buffer>, 8> buffers;
    };

    struct MaterialSharedResources : NonMovable {
        uint64_t id;

        // Bind group is the only resource directly owned by `MaterialSharedResources`.
        // The rest are stored elsewhere and reused between materials.
        wgpu::BindGroup bindGroup;

        // Uniform data containing the defaults. Copied to each `MaterialInstance` at creation.
        MaterialUniformData uniformData;

        // Keep-alive shared references to hold the required resources.
        folly::small_vector<KeepAlive<wgpu::Sampler>, 4> kaSamplers;

        // TODO: Should this store keep-alives to texture views maybe?
        folly::small_vector<KeepAlive<wgpu::Texture>, 8> kaTextures;

        // Storage buffers.
        folly::small_vector<wgpu::Buffer, 8> buffers;

        KeepAlive<MaterialLayout> kaLayout;

        MaterialSharedResources(uint64_t id) : id{id} {}

    protected:
        void
        initBindGroup(const res::MaterialResource& matRes, const ShaderProgram& program,
                      const DynamicMaterialResourceDescriptor* pDynamicBindings = nullptr);
    };

    struct SurfaceMaterialSharedResources : MaterialSharedResources {
        KeepAlive<ShaderProgram> kaColorProgram;
        KeepAlive<ShaderProgram> kaDepthProgram;
        KeepAlive<SurfacePipeline> kaPipeline;

        SurfaceMaterialSharedResources(uint64_t id, const res::MaterialResource& matRes,
                                       const MeshVertexFormat& vertexFormat);

        inline const KeepAlive<SurfacePipeline>&
        getRenderPipeline() const
        {
            return kaPipeline;
        }
    };

    struct PostProcessMaterialSharedResources : MaterialSharedResources {
        KeepAlive<ShaderProgram> kaMainProgram;
        KeepAlive<PostProcessPipeline> kaPipeline;

        PostProcessMaterialSharedResources(uint64_t id, const res::MaterialResource& matRes);

        inline const KeepAlive<PostProcessPipeline>&
        getRenderPipeline() const
        {
            return kaPipeline;
        }
    };

    struct DynamicMaterialSharedResources : MaterialSharedResources {
        KeepAlive<ShaderProgram> kaMainProgram;
        KeepAlive<DynamicPipeline> kaPipeline;

        DynamicMaterialSharedResources(uint64_t id, const res::MaterialResource& matRes,
                                       const DynamicMaterialResourceDescriptor& dynamicBindings,
                                       const MaterialConfiguration* pAdditionalConfig = nullptr);

        inline const KeepAlive<DynamicPipeline>&
        getRenderPipeline() const
        {
            return kaPipeline;
        }
    };

    template<class TSharedResources>
    class MaterialInstance {
        KeepAlive<TSharedResources> kaSharedResources;
        MaterialUniformData uniformData;

    public:
        MaterialInstance() = default;
        MaterialInstance(const MaterialInstance& other) = default;
        MaterialInstance(MaterialInstance&& other) = default;

        MaterialInstance&
        operator=(const MaterialInstance& other) = default;
        MaterialInstance&
        operator=(MaterialInstance&& other) = default;

        MaterialInstance(KeepAlive<TSharedResources> kaSharedResources) : kaSharedResources{kaSharedResources}
        {
            uniformData = kaSharedResources->uniformData;
        }

        inline bool
        isValid() const
        {
            return kaSharedResources;
        }

        inline auto
        getRenderPipeline() const
        {
            LEMON_ASSERT(isValid());
            return *kaSharedResources->getRenderPipeline();
        }

        template<MaterialUniformDataType TData>
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

    using SurfaceMaterialInstance = MaterialInstance<SurfaceMaterialSharedResources>;
    using PostProcessMaterialInstance = MaterialInstance<PostProcessMaterialSharedResources>;
    using DynamicMaterialInstance = MaterialInstance<DynamicMaterialSharedResources>;
} // namespace lemon::render

template<>
struct folly::hasher<lemon::render::MaterialResourceDescriptor> {
    using folly_is_avalanching = std::true_type;

    size_t
    operator()(const lemon::render::MaterialResourceDescriptor& data) const;
};

template<>
struct folly::hasher<lemon::render::DynamicMaterialResourceDescriptor> {
    using folly_is_avalanching = std::true_type;

    size_t
    operator()(const lemon::render::DynamicMaterialResourceDescriptor& data) const;
};
