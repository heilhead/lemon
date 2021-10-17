#include <lemon/render/MeshVertexFormat.h>
#include <lemon/render.h>
#include <lemon/resource/types/MaterialResource.h>

using namespace magic_enum::bitwise_operators;
using namespace lemon::render;
using namespace lemon::res;

void
MeshVertexFormat::setComponents(MeshComponents inComponents)
{
    components = inComponents;

    reset();

    if ((bool)(components & MeshComponents::Position)) {
        add(wgpu::VertexFormat::Float32x3);
    } else {
        skip();
    }

    if ((bool)(components & MeshComponents::Normal)) {
        add(wgpu::VertexFormat::Snorm8x4);
    } else {
        skip();
    }

    if ((bool)(components & MeshComponents::Tangent)) {
        add(wgpu::VertexFormat::Snorm8x4);
    } else {
        skip();
    }

    if ((bool)(components & MeshComponents::UV0)) {
        add(wgpu::VertexFormat::Unorm16x2);
    } else {
        skip();
    }

    if ((bool)(components & MeshComponents::UV1)) {
        add(wgpu::VertexFormat::Unorm16x2);
    } else {
        skip();
    }

    if ((bool)(components & MeshComponents::JointInfluence)) {
        add(wgpu::VertexFormat::Uint8x4);
        add(wgpu::VertexFormat::Unorm8x4);
    } else {
        skip();
        skip();
    }
}

bool
MeshVertexFormat::has(MeshComponents inComponents) const
{
    return (bool)(components & inComponents);
}

void
MeshVertexFormat::add(wgpu::VertexFormat format)
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
MeshVertexFormat::skip()
{
    locationCount++;
}

void
MeshVertexFormat::reset()
{
    stride = 0;
    attributeCount = 0;
    locationCount = 0;
}

MaterialConfiguration
MeshVertexFormat::getMeshConfig() const
{
    MaterialConfiguration config;

    LEMON_ASSERT(has(MeshComponents::Position), "invalid vertex format");

    bool bNormal = has(MeshComponents::Normal);
    bool bTangent = has(MeshComponents::Tangent);

    config.define(kShaderDefineMeshNormal, bNormal);
    config.define(kShaderDefineMeshTangent, bTangent);
    config.define(kShaderDefineMeshTangentSpace, bTangent && bNormal);

    config.define(kShaderDefineMeshTexture0, has(MeshComponents::UV0));
    config.define(kShaderDefineMeshTexture1, has(MeshComponents::UV1));

    config.define(kShaderDefineMeshSkinning, has(MeshComponents::JointInfluence));

    return config;
}
