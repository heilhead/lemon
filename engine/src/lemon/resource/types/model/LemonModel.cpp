#include <lemon/resource/types/model/LemonModel.h>
#include <lemon/utils/utils.h>
#include <lemon/shared/assert.h>

using namespace lemon;
using namespace lemon::res::model;

size_t
MeshPackedVertex::getSize(MeshComponents components)
{
    size_t size = 0;

    if ((bool)(components & MeshComponents::Position)) {
        size += sizeof(position);
    }

    if ((bool)(components & MeshComponents::Normal)) {
        size += sizeof(normal);
    }

    if ((bool)(components & MeshComponents::Tangent)) {
        size += sizeof(tangent);
    }

    if ((bool)(components & MeshComponents::UV0)) {
        size += sizeof(uv0);
    }

    if ((bool)(components & MeshComponents::UV1)) {
        size += sizeof(uv1);
    }

    if ((bool)(components & MeshComponents::JointInfluence)) {
        size += sizeof(jointIndex);
        size += sizeof(jointWeight);
    }

    return size;
}

void
MeshGPUVertexFormat::add(wgpu::VertexFormat format)
{
    auto idx = (size_t)attributeCount++;
    auto size = lemon::render::getVertexFormatSize(format);
    auto location = (size_t)locationCount++;

    LEMON_ASSERT(size > 0);

    attributes[idx].format = format;
    attributes[idx].offset = stride;
    attributes[idx].shaderLocation = location;

    stride += size;
}

void
MeshGPUVertexFormat::skip()
{
    locationCount++;
}

void
MeshGPUVertexFormat::reset()
{
    stride = 0;
    attributeCount = 0;
    locationCount = 0;
}

void
ModelMesh::updateVertexFormat()
{
    vertexFormat.reset();

    if ((bool)(components & MeshComponents::Position)) {
        vertexFormat.add(wgpu::VertexFormat::Float32x3);
    } else {
        vertexFormat.skip();
    }

    if ((bool)(components & MeshComponents::Normal)) {
        vertexFormat.add(wgpu::VertexFormat::Snorm8x4);
    } else {
        vertexFormat.skip();
    }

    if ((bool)(components & MeshComponents::Tangent)) {
        vertexFormat.add(wgpu::VertexFormat::Snorm8x4);
    } else {
        vertexFormat.skip();
    }

    if ((bool)(components & MeshComponents::UV0)) {
        vertexFormat.add(wgpu::VertexFormat::Unorm16x2);
    } else {
        vertexFormat.skip();
    }

    if ((bool)(components & MeshComponents::UV1)) {
        vertexFormat.add(wgpu::VertexFormat::Unorm16x2);
    } else {
        vertexFormat.skip();
    }

    if ((bool)(components & MeshComponents::JointInfluence)) {
        vertexFormat.add(wgpu::VertexFormat::Uint8x4);
        vertexFormat.add(wgpu::VertexFormat::Unorm8x4);
    } else {
        vertexFormat.skip();
        vertexFormat.skip();
    }
}

LemonModel::LemonModel()
{
    logger::log(__FUNCTION__);
}

LemonModel::~LemonModel()
{
    logger::log(__FUNCTION__);
}
