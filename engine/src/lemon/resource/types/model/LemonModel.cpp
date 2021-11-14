#include <lemon/resource/types/model/LemonModel.h>

using namespace lemon;
using namespace lemon::render;
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
ModelMesh::updateVertexFormat()
{
    vertexFormat.setComponents(components);
}

LemonModel::LemonModel() {}

LemonModel::~LemonModel() {}
