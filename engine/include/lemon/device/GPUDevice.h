#pragma once

#include <dawn/webgpu_cpp.h>
#include <dawn/dawn_wsi.h>
#include <dawn_native/DawnNative.h>
#include <lemon/render/ShaderCompiler.h>

namespace lemon::device {
    class Platform;
    class Window;

    class GPUDevice {
        dawn_native::Instance dawn;
        wgpu::Device device;
        wgpu::Queue queue;
        wgpu::SwapChain swapChain;
        DawnSwapChainImplementation swapChainImpl;
        render::ShaderCompiler shaderCompiler;

    public:
        GPUDevice(Platform* platform, Window* window);

        inline wgpu::Device&
        getDevice()
        {
            return device;
        }

        inline wgpu::Queue&
        getQueue()
        {
            return queue;
        }

        inline wgpu::SwapChain&
        getSwapChain()
        {
            return swapChain;
        }

        inline DawnSwapChainImplementation&
        getSwapChainImpl()
        {
            // TODO: Should this method even exist?
            return swapChainImpl;
        }

        std::unique_ptr<render::ShaderProgram>
        compileShaderProgram(uint64_t hash, const std::string& sourceCode);
    };
} // namespace lemon::device