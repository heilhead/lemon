#include <lemon/render/MeshGPUBuffer.h>
#include <lemon/resource/types/ModelResource.h>

using namespace lemon;
using namespace lemon::render;

MeshGPUBuffer
lemon::render::createMeshGPUBuffer(const res::model::ModelMesh* pMesh)
{
    auto& device = RenderManager::get()->getDevice();
    MeshGPUBuffer data;

    data.vertexBuffer =
        createBufferFromData(device, pMesh->vertexData, pMesh->vertexData.size(), wgpu::BufferUsage::Vertex);
    data.indexBuffer =
        createBufferFromData(device, pMesh->indexData, pMesh->indexData.size(), wgpu::BufferUsage::Index);

    data.indexCount = pMesh->indexCount;
    data.indexFormat = pMesh->indexFormat;

    return data;
}
