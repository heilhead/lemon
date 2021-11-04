#pragma once

namespace lemon::res::model {
    struct ModelMesh;
}

namespace lemon::render {
    struct MeshGPUBuffer {
        wgpu::Buffer vertexBuffer;
        wgpu::Buffer indexBuffer;
        size_t indexCount;
        wgpu::IndexFormat indexFormat;
    };

    MeshGPUBuffer
    createMeshGPUBuffer(const res::model::ModelMesh* pMesh);
} // namespace lemon::render
