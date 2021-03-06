#include <lemon/device/GPUDevice.h>
#include <lemon/device/Platform.h>
#include <lemon/device/Window.h>

using namespace lemon::device;
using namespace lemon::render;
using namespace lemon;

void
printDeviceError(WGPUErrorType errorType, gsl::czstring<> message, void*)
{
    gsl::czstring<> errorTypeName = "";
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
        LEMON_UNREACHABLE("invalid WebGPU error type: ", (int)errorType);
        return;
    }

    logger::err("WebGPU device error: ", errorTypeName, ": ", message);
}

GPUDevice::GPUDevice(Platform* platform, Window* window)
{
    dawn.SetPlatform(platform);
    dawn.DiscoverDefaultAdapters();

    dawn_native::Adapter backendAdapter;
    {
        logger::log("discovering adapters...");

        auto adapters = dawn.GetAdapters();
        for (dawn_native::Adapter& adapter : adapters) {
            wgpu::AdapterProperties properties;
            adapter.GetProperties(&properties);

            logger::log("found adapter: ", properties.name, " backend: ", properties.backendType);

            if (properties.backendType == wgpu::BackendType::D3D12) {
                backendAdapter = adapter;
                logger::log("using adapter: ", properties.name, " backend: ", properties.backendType);
                break;
            }
        }

        LEMON_ASSERT(backendAdapter, "no suitable adapter found");
    }

    dawn_native::DeviceDescriptor desc;
    desc.forceEnabledToggles.push_back("disable_symbol_renaming");
    desc.forceEnabledToggles.push_back("emit_hlsl_debug_symbols");
    desc.forceEnabledToggles.push_back("turn_off_vsync");
    // desc.forceEnabledToggles.push_back("skip_validation");
    // desc.forceEnabledToggles.push_back("use_user_defined_labels_in_backend");
    // desc.forceEnabledToggles.push_back("dump_shaders");

    auto backendDevice = backendAdapter.CreateDevice(&desc);

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

    renderManager.init(device);
}
