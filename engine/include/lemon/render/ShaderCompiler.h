#pragma once

#include <lemon/render/ShaderProgram.h>
#include <dawn/webgpu_cpp.h>

namespace lemon::render {
    class ShaderCompiler {
        wgpu::Device* pDevice;

    public:
        ShaderCompiler() : pDevice{} {}
        ShaderCompiler(wgpu::Device* pDevice) : pDevice{pDevice} {}

        std::unique_ptr<ShaderProgram>
        compile(uint64_t hash, const std::string& sourceCode);

    private:
        static void
        compilationInfoCallback(WGPUCompilationInfoRequestStatus status,
                                WGPUCompilationInfo const* compilationInfo, void* userdata);
    };
} // namespace lemon::render