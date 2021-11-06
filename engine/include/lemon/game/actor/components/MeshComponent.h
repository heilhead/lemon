#pragma once

#include <lemon/game/actor/ActorComponent.h>
#include <lemon/render/material/MaterialManager.h>
#include <lemon/render/MeshGPUBuffer.h>
#include <lemon/render/RenderQueue.h>
#include <lemon/resource/types/ModelResource.h>

namespace lemon::game {
    class MeshComponent : public RenderableComponent, public render::MeshRenderProxyOwner {
        static constexpr auto kMaxInlineMeshes = 4;
        static constexpr auto kMaxInlineMaterials = 4;

        struct MeshDescriptor {
            size_t index;
            render::MeshComponents components;
            render::MeshVertexFormat format;
            render::MeshGPUBuffer buffer;
            std::optional<render::MaterialInstance> material;
            render::MeshRenderHandle renderHandle;
            bool bMaterialDirty;
            uint8_t materialIndex;

            MeshDescriptor(size_t index, const res::ModelResource::MeshWrapper& meshWrapper,
                           const res::MaterialResource* pMaterial);

            void
            createMaterialInstance(const res::MaterialResource* pMaterial);

            inline bool
            isValid()
            {
                return material.has_value();
            }

            inline bool
            hasRenderHandle()
            {
                return !renderHandle.isEmpty();
            }
        };

        folly::small_vector<MeshDescriptor, kMaxInlineMeshes> meshes;
        folly::small_vector<const res::MaterialResource*, kMaxInlineMaterials> materials;

    public:
        virtual void
        setModel(const res::ResourceLocation& loc);

        virtual void
        setMaterial(uint8_t index, const res::ResourceLocation& loc);

        void
        updateRenderProxy(render::MeshRenderProxy& proxy) override;

        template<std::semiregular TData>
        inline void
        setMaterialParameter(uint8_t materialIndex, StringID paramID, const TData& value)
        {
            LEMON_ASSERT(hasMaterial(materialIndex));

            for (auto& mesh : meshes) {
                if (mesh.isValid() && mesh.materialIndex == materialIndex) {
                    (*mesh.material).getUniformData().setData(paramID, value);
                    mesh.bMaterialDirty = true;
                }
            }
        }

    protected:
        virtual void
        updateModel(const res::ModelResource::Model* pModel);

        virtual void
        updateMaterial(uint8_t index, const res::MaterialResource* pMaterial);

        virtual void
        addRenderProxy(MeshDescriptor& mesh);

        virtual void
        removeRenderProxy(MeshDescriptor& mesh);

    private:
        bool
        hasMaterial(uint8_t index) const;

        const res::MaterialResource*
        getMaterial(uint8_t index) const;
    };

    class StaticMeshComponent : public MeshComponent {
        // void
        // updateModel(const res::ModelResource::Model* pModel) override;

        // void
        // updateMaterial(uint8_t index, const res::MaterialResource* pMaterial) override;
    };

    class SkeletalMeshComponent : public MeshComponent {
        // void
        // updateModel(const res::ModelResource::Model* pModel) override;

        // void
        // updateMaterial(uint8_t index, const res::MaterialResource* pMaterial) override;
    };
} // namespace lemon::game
