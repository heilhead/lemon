#include <lemon/device/GPUDevice.h>
#include <lemon/device/Platform.h>
#include <lemon/device/Window.h>
#include <lemon/utils/utils.h>
#include <cassert>
#include <dawn/dawn_proc.h>
#include <dawn/webgpu_cpp_print.h>
#include <dawn_native/D3D12Backend.h>

using namespace lemon::device;
using namespace lemon::render;
using namespace lemon;

void
printDeviceError(WGPUErrorType errorType, const char* message, void*)
{
    const char* errorTypeName = "";
    switch (errorType) {
    case WGPUErrorType_Validation:
        errorTypeName = "Validation";
        break;
    case WGPUErrorType_OutOfMemory:
        errorTypeName = "Out of memory";
        break;
    case WGPUErrorType_Unknown:
        errorTypeName = "Unknown";
        break;
    case WGPUErrorType_DeviceLost:
        errorTypeName = "Device lost";
        break;
    default:
        assert(false);
        return;
    }

    utils::logErr("WebGPU device error: ", errorTypeName, ": ", message);
}

GPUDevice::GPUDevice(Platform* platform, Window* window)
{
    dawn.SetPlatform(platform);
    dawn.DiscoverDefaultAdapters();

    dawn_native::Adapter backendAdapter;
    {
        utils::log("discovering adapters...");

        auto adapters = dawn.GetAdapters();
        for (dawn_native::Adapter& adapter : adapters) {
            wgpu::AdapterProperties properties;
            adapter.GetProperties(&properties);

            utils::log("found adapter: ", properties.name, " backend: ", properties.backendType);

            if (properties.backendType == wgpu::BackendType::D3D12) {
                backendAdapter = adapter;
                utils::log("using adapter: ", properties.name, " backend: ", properties.backendType);
                break;
            }
        }

        assert(backendAdapter && "no suitable adapter found");
    }

    auto backendDevice = backendAdapter.CreateDevice();

    {
        const auto& backendProcs = dawn_native::GetProcs();

        dawnProcSetProcs(&backendProcs);
        backendProcs.deviceSetUncapturedErrorCallback(backendDevice, printDeviceError, nullptr);

        device = wgpu::Device::Acquire(backendDevice);
        queue = device.GetQueue();
    }

    {
        swapChainImpl =
            dawn_native::d3d12::CreateNativeSwapChainImpl(backendDevice, window->getContextHandle());
        wgpu::SwapChainDescriptor swapChainDesc;
        swapChainDesc.implementation = reinterpret_cast<uint64_t>(&swapChainImpl);
        swapChain = device.CreateSwapChain(nullptr, &swapChainDesc);

        auto textureFormat = static_cast<wgpu::TextureFormat>(
            dawn_native::d3d12::GetNativeSwapChainPreferredFormat(&swapChainImpl));
        auto [wndWidth, wndHeight] = window->getSize();
        swapChain.Configure(textureFormat, wgpu::TextureUsage::RenderAttachment, wndWidth, wndHeight);
    }

    renderManager.setDevice(device);
}