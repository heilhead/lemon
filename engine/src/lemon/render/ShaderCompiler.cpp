#include <lemon/render/ShaderCompiler.h>
#include <lemon-shader/ShaderReflection.h>

using namespace lemon::render;
using namespace lemon::shader;

std::unique_ptr<ShaderProgram>
ShaderCompiler::compile(const std::string& sourceCode)
{
    wgpu::ShaderModuleWGSLDescriptor wgslDesc;
    wgslDesc.source = sourceCode.c_str();

    wgpu::ShaderModuleDescriptor desc;
    desc.nextInChain = &wgslDesc;

    auto pProgram = std::make_unique<ShaderProgram>();
    pProgram->shaderModule = pDevice->CreateShaderModule(&desc);
    pProgram->shaderModule.GetCompilationInfo(compilationInfoCallback, pProgram.get());

    if (pProgram->isValid()) {
        std::string path = "<unknown>";
        ShaderReflection refl;
        refl.init(path, sourceCode);

        pProgram->reflection = std::move(refl.getBindingReflection());
    }

    return pProgram;
}

void
ShaderCompiler::compilationInfoCallback(WGPUCompilationInfoRequestStatus status,
                                        WGPUCompilationInfo const* compilationInfo, void* userdata)
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