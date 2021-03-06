#pragma once

#include <lemon/render/RenderManager.h>

namespace lemon::device {
    class Platform;
    class Window;

    class GPUDevice {
        dawn_native::Instance dawn;
        wgpu::Device device;
        wgpu::Queue queue;
        wgpu::SwapChain swapChain;
        DawnSwapChainImplementation swapChainImpl;
        render::RenderManager renderManager;

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

        inline wgpu::TextureFormat
        getColorTargetFormat() const
        {
            return static_cast<wgpu::TextureFormat>(
                dawn_native::d3d12::GetNativeSwapChainPreferredFormat(&swapChainImpl));
        }
    };
} // namespace lemon::device
