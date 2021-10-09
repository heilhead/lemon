#include <lemon/render/RenderManager.h>
#include <lemon/render/material/MaterialLayout.h>
#include <lemon/resource/types/MaterialResource.h>
#include <folly/small_vector.h>
#include <lemon-shader/ShaderReflection.h>

using namespace lemon::render;
using namespace lemon::shader;
using namespace lemon::res;

MaterialUniformLayout::MaterialUniformLayout(const ShaderProgram& program, uint32_t bindGroup)
{
    std::ostringstream str;
    uint32_t itemCount = 0;

    for (auto& res : program.getReflection()) {
        if (res.resourceType != ResourceType::kUniformBuffer) {
            continue;
        }

        if (res.bindGroup != bindGroup) {
            continue;
        }

        LEMON_ASSERT_MSG(res.members.size() <= kMaterialMaxUniformMembers, "too many uniform properties");

        auto& uniform = uniforms[uniformCount];
        uniform.id = res.id;
        uniform.size = res.size;

        for (auto& m : res.members) {
            auto& member = uniform.members[uniform.memberCount];

            member.id = lemon::sid(res.name + kMaterialUniformPropertyDelimiter + m.name);

            // `totalSize` here is the base uniform offset.
            member.offset = totalSize + m.offset;
            member.size = m.size;

            itemCount++;
            uniform.memberCount++;

            LEMON_ASSERT_MSG(itemCount <= kMaxItems, "too many uniform bindings");
        }

        totalSize += res.size;
        uniformCount++;
    }
}

wgpu::ShaderStage
convertShaderStage(PipelineStage value)
{
    wgpu::ShaderStage result = wgpu::ShaderStage::None;

    if ((bool)(value & PipelineStage::kVertex)) {
        result |= wgpu::ShaderStage::Vertex;
    }

    if ((bool)(value & PipelineStage::kFragment)) {
        result |= wgpu::ShaderStage::Fragment;
    }

    if ((bool)(value & PipelineStage::kCompute)) {
        result |= wgpu::ShaderStage::Compute;
    }

    return result;
}

wgpu::TextureSampleType
convertSampleType(SampledKind value)
{
    switch (value) {
    case SampledKind::kFloat:
        return wgpu::TextureSampleType::Float;
    case SampledKind::kSInt:
        return wgpu::TextureSampleType::Sint;
    case SampledKind::kUInt:
        return wgpu::TextureSampleType::Uint;
    default:
        return wgpu::TextureSampleType::Undefined;
    }
}

wgpu::TextureViewDimension
convertViewDimension(TextureDimension value)
{
    switch (value) {
    case TextureDimension::k1d:
        return wgpu::TextureViewDimension::e1D;
    case TextureDimension::k2d:
        return wgpu::TextureViewDimension::e2D;
    case TextureDimension::k2dArray:
        return wgpu::TextureViewDimension::e2DArray;
    case TextureDimension::k3d:
        return wgpu::TextureViewDimension::e3D;
    case TextureDimension::kCube:
        return wgpu::TextureViewDimension::Cube;
    case TextureDimension::kCubeArray:
        return wgpu::TextureViewDimension::CubeArray;
    default:
        return wgpu::TextureViewDimension::Undefined;
    }
}

wgpu::BindGroupLayout
createBindGroupLayout(const MaterialResource& material, const ShaderProgram& program, uint8_t bindGroupIndex)
{
    folly::small_vector<wgpu::BindGroupLayoutEntry, 8> entries;

    for (auto& resDesc : program.getReflection()) {
        if (resDesc.bindGroup != bindGroupIndex) {
            continue;
        }

        wgpu::BindGroupLayoutEntry entry;
        entry.binding = resDesc.binding;
        entry.visibility = convertShaderStage(resDesc.stage);

        switch (resDesc.resourceType) {
        case ResourceType::kUniformBuffer:
            entry.buffer.type = wgpu::BufferBindingType::Uniform;
            entry.buffer.hasDynamicOffset = true;
            entry.buffer.minBindingSize = resDesc.size;
            break;
        case ResourceType::kSampledTexture:
            entry.texture.sampleType = convertSampleType(resDesc.sampledKind);
            entry.texture.viewDimension = convertViewDimension(resDesc.dim);
            entry.texture.multisampled = false;
            break;
        case ResourceType::kSampler:
            // TODO: Use `SamplerDescriptor` to figure out sampling parameters.
            entry.sampler.type = wgpu::SamplerBindingType::Filtering;
            break;
        default:
            LEMON_TODO();
        }

        entries.emplace_back(entry);
    }

    wgpu::BindGroupLayoutDescriptor desc;
    desc.entryCount = entries.size();
    desc.entries = entries.data();

    return RenderManager::get()->getDevice().CreateBindGroupLayout(&desc);
}

MaterialLayout::MaterialLayout(const MaterialResource& material, const ShaderProgram& program,
                               uint8_t bindGroupIndex)
    : bindGroupLayout{std::move(createBindGroupLayout(material, program, bindGroupIndex))},
      uniformLayout{program, bindGroupIndex}
{
}