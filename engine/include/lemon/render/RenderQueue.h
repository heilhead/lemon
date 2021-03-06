#pragma once

#include <lemon/render/material/MaterialInstance.h>

namespace lemon::render {
    class MeshRenderProxyOwner;
    class LightRenderProxyOwner;

    LEMON_SLOT_MAP_HANDLE(MeshRenderHandle);
    LEMON_SLOT_MAP_HANDLE(LightRenderHandle);

    struct MeshRenderProxy {
        MeshRenderProxyOwner* pOwner{nullptr};
        size_t meshIndex;
        SurfaceMaterialInstance material;
        wgpu::Buffer vertexBuffer;
        wgpu::Buffer indexBuffer;
        size_t indexCount;
        wgpu::IndexFormat indexFormat;
        glm::f32mat4 matrix;
        uint32_t matrixGeneration;
    };

    struct LightRenderProxy {
        LightRenderProxyOwner* pOwner{nullptr};
    };

    class MeshRenderProxyOwner {
    public:
        virtual void
        updateRenderProxy(MeshRenderProxy& proxy) = 0;
    };

    class LightRenderProxyOwner {
    public:
        virtual void
        updateRenderProxy(LightRenderProxy& proxy) = 0;
    };

    using MeshRenderList = SlotMap<MeshRenderProxy, MeshRenderHandle>;
    using LightRenderList = SlotMap<LightRenderProxy, LightRenderHandle>;

    class RenderQueue {
        static constexpr auto kMeshStoreCapacity = 1024;
        static constexpr auto kLightStoreCapacity = 256;

        MeshRenderList meshes;
        LightRenderList lights;

    public:
        RenderQueue();

        MeshRenderHandle
        addMeshProxy(MeshRenderProxy&& proxy)
        {
            return meshes.insert(std::move(proxy));
        }

        void
        removeMeshProxy(MeshRenderHandle handle)
        {
            meshes.remove(handle);
        }

        MeshRenderList&
        getMeshes()
        {
            return meshes;
        }

        LightRenderList
        getLights()
        {
            return lights;
        }
    };
} // namespace lemon::render
