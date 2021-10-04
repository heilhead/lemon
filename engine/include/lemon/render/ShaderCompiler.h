#pragma once

#include <lemon/render/ShaderProgram.h>
#include <lemon/shared/AtomicCache.h>
#include <dawn/webgpu_cpp.h>

namespace lemon::render {
    class ShaderCompiler {
        static constexpr uint8_t kUserBindGroupIndex = 1;

    public:
        std::unique_ptr<ShaderProgram>
        compile(uint64_t hash, const std::string& sourceCode);

        const wgpu::BindGroupLayout&
        getBindGroupLayout(const ShaderProgram& program);

    private:
        static void
        compilationInfoCallback(WGPUCompilationInfoRequestStatus status,
                                const WGPUCompilationInfo* compilationInfo, void* userdata);
    };
} // namespace lemon::render