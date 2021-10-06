#include <lemon/render/material/ShaderCompiler.h>
#include <lemon/render/RenderManager.h>
#include <folly/small_vector.h>
#include <magic_enum.hpp>

using namespace lemon::render;

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