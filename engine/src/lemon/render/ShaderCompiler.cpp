#include <lemon/render/ShaderCompiler.h>
#include <lemon/render/RenderManager.h>
#include <folly/small_vector.h>
#include <magic_enum.hpp>

using namespace lemon::render;
using namespace lemon::shader;
using namespace magic_enum::bitwise_operators;

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

// TODO: Reflection data is likely not enough here. Consider passing some of the `MaterialResource`
// descriptors, e.g. `SamplerDescriptor`.
wgpu::BindGroupLayout
createBindGroupLayout(uint8_t bindGroupIndex, const ShaderProgram* pProgram)
{
    folly::small_vector<wgpu::BindGroupLayoutEntry, 8> entries;

    for (auto& resDesc : pProgram->getReflection()) {
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

std::unique_ptr<ShaderProgram>
ShaderCompiler::compile(uint64_t hash, const std::string& sourceCode)
{
    wgpu::ShaderModuleWGSLDescriptor wgslDesc;
    wgslDesc.source = sourceCode.c_str();

    wgpu::ShaderModuleDescriptor desc;
    desc.nextInChain = &wgslDesc;

    auto pProgram = std::make_unique<ShaderProgram>(hash);
    pProgram->shaderModule = RenderManager::get()->getDevice().CreateShaderModule(&desc);
    pProgram->shaderModule.GetCompilationInfo(compilationInfoCallback, pProgram.get());

    if (pProgram->isValid()) {
        pProgram->createReflection("<unknown>", sourceCode);
    }

    return pProgram;
}

const wgpu::BindGroupLayout&
ShaderCompiler::getBindGroupLayout(const ShaderProgram& program)
{
    auto [pBindGroupLayout, bInserted] =
        ResourceCache::bindGroupLayout().findOrInsert(program.reflectionHash, [&]() {
            return new wgpu::BindGroupLayout(std::move(createBindGroupLayout(kUserBindGroupIndex, &program)));
        });

    return pBindGroupLayout.get();
}

void
ShaderCompiler::compilationInfoCallback(WGPUCompilationInfoRequestStatus status,
                                        const WGPUCompilationInfo* compilationInfo, void* userdata)
{
    auto* pProgram = reinterpret_cast<ShaderProgram*>(userdata);
    pProgram->valid = status == WGPUCompilationInfoRequestStatus_Success;

    for (int i = 0; i < compilationInfo->messageCount; i++) {
        auto& gpuMsg = compilationInfo->messages[i];

        ShaderProgram::DiagnosticMessage msg{.message = gpuMsg.message,
                                             .type = gpuMsg.type,
                                             .lineNum = gpuMsg.lineNum,
                                             .linePos = gpuMsg.linePos,
                                             .offset = gpuMsg.offset,
                                             .length = gpuMsg.length};

        pProgram->diagnostics.emplace_back(msg);
    }
}