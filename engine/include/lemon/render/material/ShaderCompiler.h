#pragma once

#include <lemon/render/material/ShaderProgram.h>

namespace lemon::render {
    class ShaderCompiler {
    public:
        std::unique_ptr<ShaderProgram>
        compile(uint64_t hash, const std::string& sourceCode);

    private:
        static void
        compilationInfoCallback(WGPUCompilationInfoRequestStatus status,
                                const WGPUCompilationInfo* compilationInfo, void* userdata);
    };
} // namespace lemon::render
